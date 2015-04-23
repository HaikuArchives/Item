/* Newsgroup.cpp */

#include "Newsgroup.h"
#include "SubjectSubjectTree.h"
#include "SubjectFilterTree.h"
#include "ArticleSet.h"
#include "Subject.h"
#include "Article.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "TextReader.h"
#include "NewsgroupWind.h"
#include "Filters.h"
#include "FilterGroup.h"
#include "Task.h"
#include "NNTPApp.h"
#include "Newsrc.h"
#include "Prefs.h"
#include "FileNames.h"
#include <stdio.h>
#include <File.h>
#include <NodeInfo.h>
#include <Path.h>
#include <Directory.h>
#include <FindDirectory.h>

const unsigned int Newsgroup::maxHeaders = 10000;


Newsgroup::Newsgroup(string_slice groupNameIn, Newsrc* newsrcIn)
	: groupName(groupNameIn), newsrc(newsrcIn),
	  numUnreadArticles(0), cachedHeaderInfo(NULL), wind(NULL)
{
	readArticles = new ArticleSet();
	subjectTree = new SubjectSubjectTree();
	filterTree = new SubjectFilterTree();
	headersResponses = new BList();
}


Newsgroup::~Newsgroup()
{
	if (wind)
		wind->Close();
	Close();
	delete readArticles;
	delete subjectTree;
	delete filterTree;
	delete headersResponses;
	delete cachedHeaderInfo;
}


void Newsgroup::SetAvailableArticles(int firstArticle, int lastArticle)
{
	availableArticles = IntRange(firstArticle, lastArticle);

	// mark expired articles as read
	IntRange expiredArticles(1, firstArticle - 1);
	if (expiredArticles.IsValid())
		readArticles->AddRange(expiredArticles);

	CalcNumUnreadArticles();
}


void Newsgroup::SetReadArticles(ArticleSet* newReadArticles)	// assumes ownership
{
	delete readArticles;
	readArticles = newReadArticles;

	// mark expired articles as read also
	IntRange expiredArticles(1, availableArticles.min - 1);
	if (expiredArticles.IsValid())
		readArticles->AddRange(expiredArticles);

	CalcNumUnreadArticles();
}


status_t Newsgroup::ReadHeaders(NNTPConnection* connection, ArticleSet* cachedArticles, Task* task, volatile bool& stopRequested)	// takes ownership of "cachedArticles"
{
	status_t result = B_NO_ERROR;

	// copy "readArticles" into "cachedArticles", using that as the set of unneeded articles
	ArticleSet* unneededArticles;
	if (cachedArticles) {
		// use "cachedArticles" as "unneededArticles"
		unneededArticles = cachedArticles;
		// copy "readArticles" into it
		unneededArticles->Incorporate(readArticles);
		}
	else
		unneededArticles = readArticles;	// use "readArticles" directly
	task->SetProgressMax(availableArticles.Size() - unneededArticles->NumArticlesIn(availableArticles));

	IntRange lastReadRange(-1, -1);
	int numRanges = unneededArticles->NumRanges();
	for (int i=0; i<numRanges; i++) {
		if (stopRequested)
			break;
		IntRange readRange = unneededArticles->RangeAt(i);

		// unread range is between readRanges
		IntRange unreadRange(lastReadRange.max + 1, readRange.min - 1);

		// read the range
		IntRange unreadAvailable = unreadRange & availableArticles;
		if (unreadAvailable.IsValid()) {
			result = GetOneHeaderRange(connection, unreadAvailable, task);
			if (result != B_NO_ERROR)
				goto exit;
			}

		lastReadRange = readRange;
		}
	// handle last unread range
	{
	IntRange lastUnreadRange(lastReadRange.max + 1, availableArticles.max);
	IntRange lastUnreadAvailable = lastUnreadRange & availableArticles;
	if (lastUnreadAvailable.IsValid() && !stopRequested) {
		result = GetOneHeaderRange(connection, lastUnreadAvailable, task);
		if (result != B_NO_ERROR)
			goto exit;
		}
	}

exit:
	if (unneededArticles != readArticles)
		delete unneededArticles;
	return B_NO_ERROR;
}


void Newsgroup::ArticleRead(int articleNo)
{
	readArticles->AddArticle(articleNo);
}


void Newsgroup::ArticleUnread(int articleNo)
{
	readArticles->RemoveArticle(articleNo);
}


void Newsgroup::ReadArticlesChanged()
{
	CalcNumUnreadArticles();
}


void Newsgroup::Close()
{
	int i;

	// write the article cache, but only if headers have been loaded
	if (subjectTree->NumSubjects() > 0)
		WriteArticleCache();

	// delete all subjects
	int numSubjects = subjectTree->NumSubjects();
	for (i=0; i<numSubjects; i++)
		delete subjectTree->SubjectAt(i);

	// clear indexes
	subjectTree->DeleteAllObjects();
	filterTree->DeleteAllObjects();
	// delete responses
	int numResponses = headersResponses->CountItems();
	for (int i=0; i<numResponses; i++)
		delete (NNTPResponse*) headersResponses->ItemAt(i);
	headersResponses->MakeEmpty();
	// delete cached header info
	delete cachedHeaderInfo;
	cachedHeaderInfo = NULL;
}


void Newsgroup::WriteArticleCache()
{
	string_slice eol("\n");
	string_slice tab("\t");

	// open the file
	BFile* file = OpenHeaderCacheFile(B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file == NULL || file->InitCheck() != B_NO_ERROR)
		return;
	BNodeInfo(file).SetType("text/plain");

	// write all the unread subjects/articles out
	int numSubjects = subjectTree->NumSubjects();
	for (int subjIdx=0; subjIdx<numSubjects; subjIdx++) {
		Subject* subject = subjectTree->SubjectAt(subjIdx);
		if (subject->AllRead())
			continue;

		// write the subject
		string_slice subjectStr = subject->GetSubject();
		file->Write(subjectStr.begin(), subjectStr.length());
		file->Write(eol.begin(), eol.length());

		// write the unread articles
		int numArticles = subject->NumArticles();
		for (int articleIdx=0; articleIdx<numArticles; articleIdx++) {
			Article* article = subject->ArticleAt(articleIdx);
			if (article->IsRead())
				continue;

			// write "\t<articleNo>\t<author>\t<date>\t<lines>"
			file->Write(tab.begin(), tab.length());
			// articleNo
			qstring articleNo(article->ArticleNo());
			file->Write(articleNo.data(), articleNo.length());
			file->Write(tab.begin(), tab.length());
			// author
			string_slice author = article->Author();
			file->Write(author.begin(), author.length());
			file->Write(tab.begin(), tab.length());
			// date
			time_t articleDate = article->Date();
			string_slice date = ctime(&articleDate);
			date.resize(date.length() - 1);	// trim off the trailing '\n'
			file->Write(date.begin(), date.length());
			file->Write(tab.begin(), tab.length());
			// lines
			qstring lines(article->Lines());
			file->Write(lines.data(), lines.length());
			// EOL
			file->Write(eol.begin(), eol.length());
			}
		}

	delete file;
}


ArticleSet* Newsgroup::ReadArticleCache()
{
	// lock
	if (wind)
		wind->Lock();

	// get info
	bool expandSubjects = Prefs()->GetBoolPref("expandSubjectsByDefault");

	// open the file
	BFile* file = OpenHeaderCacheFile(B_READ_ONLY);
	if (file == NULL || file->InitCheck() != B_NO_ERROR) {
		if (wind)
			wind->Unlock();
		return NULL;
		}

	// read it into memory
	off_t fileSize = 0;
	file->GetSize(&fileSize);
	cachedHeaderInfo = new char[fileSize];
	file->ReadAt(0, cachedHeaderInfo, fileSize);
	delete file;

	// read in the headers
	ArticleSet* cachedArticles = new ArticleSet();
	TextReader reader(string_slice(cachedHeaderInfo, cachedHeaderInfo + fileSize));
	Subject* curSubject = NULL;
	Filters* filters = ((NNTPApp*) be_app)->GetFilters();
	for (; !reader.AtEOF(); reader.NextLine()) {
		// if it's a subject line, set up new subject
		string_slice subject = reader.NextTabField();
		if (subject.length() > 0) {
			// finish old subject
			if (curSubject) {
				if (curSubject->NumArticles() > 0)
					subjectTree->Insert(curSubject);
				else
					delete curSubject;
				}
			// start new subject
			curSubject = new Subject(subject);
			if (expandSubjects)
				curSubject->SetExpanded(true);
			}

		// otherwise it's an article line
		else if (curSubject != NULL) {
			// make the article, but only if it's not read
			int articleNo = qstring(reader.NextTabField()).asInt();
			if (readArticles->ContainsArticle(articleNo))
				continue;
			string_slice author = reader.NextTabField();
			time_t date = SliceToDate(reader.NextTabField());
			int lines = qstring(reader.NextTabField()).asInt();
			Article* article = new Article(articleNo, author, date, lines);

			// filter the article, and add or delete it
			filters->GetGlobalFilters()->ApplyFilters(article, curSubject->GetSubject());
			FilterGroup* filterGroup = filters->GetFilterGroup(groupName);
			if (filterGroup)
				filterGroup->ApplyFilters(article, curSubject->GetSubject());
			if (!article->IsRead()) {
				curSubject->AddArticle(article);
				cachedArticles->AddArticle(articleNo);
				}
			else {
				readArticles->AddArticle(articleNo);
				delete article;
				}
			}
		}
	// finish final subject
	if (curSubject) {
		if (curSubject->NumArticles() > 0)
			subjectTree->Insert(curSubject);
		else
			delete curSubject;
		}

	// build the filterTree
	int numSubjects = subjectTree->NumObjects();
	for (int i=0; i<numSubjects; i++)
		filterTree->Insert(subjectTree->SubjectAt(i));

	// refresh & unlock
	if (wind) {
		wind->NumSubjectsChanged();
		wind->Unlock();
		}

	return cachedArticles;
}


void Newsgroup::RemoveReadArticles()
{
	int numSubjects = subjectTree->NumSubjects();
	for (int i=0; i<numSubjects; ) {
		Subject* subject = subjectTree->SubjectAt(i);
		int numArticles = subject->NumArticles();
		int numUnread = subject->NumUnreadArticles();
		if (numUnread == 0) {
			// all are read--remove the whole subject
			if (wind)
				wind->Lock();
			subjectTree->Remove(subject);
			filterTree->Remove(subject);
			if (wind)
				wind->NumSubjectsChanged();
			if (wind)
				wind->Unlock();
			// adjust the loop
			numSubjects -= 1;
			}
		else if (numUnread != numArticles) {
			// some are read; remove them (including a re-filter)
			if (wind)
				wind->Lock();
			filterTree->Remove(subject);
			int numArticles = subject->NumArticles();
			for (int articleIndex = 0; articleIndex < numArticles; ) {
				Article* article = subject->ArticleAt(articleIndex);
				if (article->IsRead()) {
					subject->RemoveArticle(article);
					numArticles -= 1;
					}
				else
					++articleIndex;
				}
			filterTree->Insert(subject);
			if (wind)
				wind->SubjectChanged(subject);
			if (wind)
				wind->Unlock();
			// go to the next one
			++i;
			}
		else
			++i;
		}
}


string_slice Newsgroup::Name()
{
	return groupName;
}


int Newsgroup::NumUnreadArticles()
{
	return numUnreadArticles;
}


bool Newsgroup::HaveAvailableArticlesInfo()
{
	return availableArticles.IsValid();
}


ArticleSet* Newsgroup::ReadArticles()
{
	return readArticles;
}


ListableTree* Newsgroup::SubjectTree()
{
	return subjectTree;
}


ListableTree* Newsgroup::FilterTree()
{
	return filterTree;
}


void Newsgroup::AttachedToWindow(NewsgroupWind* window)
{
	wind = window;
}


NewsgroupWind* Newsgroup::Window()
{
	return wind;
}


void Newsgroup::SetNewsrc(Newsrc* newNewsrc)
{
	newsrc = newNewsrc;
}


status_t Newsgroup::GetOneHeaderRange(NNTPConnection* connection, IntRange range, Task* task)
{
	NNTPResponse* response;
	bool expandSubjects = Prefs()->GetBoolPref("expandSubjectsByDefault");

	// send the XOVER command
	char cmd[256];
	sprintf(cmd, "xover %d-%d", range.min, range.max);
	status_t result = connection->SendCommand(cmd);
	if (result != B_NO_ERROR)
		return result;
	result = connection->GetResponse(&response);
	if (result != 224)
		return result;
	response->NextLine();

	// add the subjects and articles
	ArticleSet expiredArticles;
	expiredArticles.AddRange(range);
	bool gotHeaders = false;
	Filters* filters = ((NNTPApp*) be_app)->GetFilters();
	int curProgress = task->CurProgress();
	while (!response->AtEOF()) {
		// get the subject from the XOVER response
		int articleNo = response->NextIntTabField();
		string_slice subject = response->NextTabField();
		string_slice author = response->NextTabField();
		time_t date = SliceToDate(response->NextTabField());
		response->NextTabField();	// skip message-id
		response->NextTabField();	// skip references
		response->NextTabField();	// skip byte count
		int numLines = response->NextIntTabField();
		response->NextLine();
		if (subject.length() == 0)
			continue;

		// trim "Re:"
		subject = TrimSubject(subject);

		// lock
		if (wind)
			wind->Lock();

		// create and filter the article
		Article* article = new Article(articleNo, author, date, numLines);
		filters->GetGlobalFilters()->ApplyFilters(article, subject);
		FilterGroup* filterGroup = filters->GetFilterGroup(groupName);
		if (filterGroup)
			filterGroup->ApplyFilters(article, subject);

		// delete the article if killed by filter
		if (article->IsRead()) {
			delete article;
			readArticles->AddArticle(articleNo);
			}

		// otherwise add it in
		else {
			// add to the subject trees if it's not already there
			Subject* subjectObj = subjectTree->Find(subject);
			if (subjectObj == NULL) {
				subjectObj = new Subject(subject);
				if (expandSubjects)
					subjectObj->SetExpanded(true);
				subjectObj->AddArticle(article);
				subjectTree->Insert(subjectObj);
				filterTree->Insert(subjectObj);
				if (wind)
					wind->NumSubjectsChanged();
				}

			// otherwise add the article; it may move the subject in the filterTree
			else {
				filterTree->Remove(subjectObj);
				subjectObj->AddArticle(article);
				filterTree->Insert(subjectObj);
				if (wind)
					wind->SubjectChanged(subjectObj);
				}

			// refresh display
/*** no longer needed?  since we do it if not adding a new subject?
			if (wind)
				wind->SubjectChanged(subjectObj);
***/

			// indicate that we need to keep the response
			gotHeaders = true;
			}

		// mark as not expired
		expiredArticles.RemoveArticle(articleNo);

		// update progress
		curProgress += 1;
		if (task->IsReadyForUpdate())
			task->SetProgress(curProgress);

		// unlock
		if (wind)
			wind->Unlock();
		}
	task->SetProgress(curProgress);

	// add to the "headersResponses"
	if (gotHeaders)
		headersResponses->AddItem(response);
	else
		delete response;

	// mark expired articles as read
	readArticles->Incorporate(&expiredArticles);

	return B_NO_ERROR;
}


string_slice Newsgroup::TrimSubject(string_slice subject)
{
	const char* p = subject.begin();
	const char* stopper = subject.end();
	while (p < stopper) {
		// skip whitespace
		for (; p < stopper; p++) {
			if (*p != ' ' && *p != '\t')
				break;
			}
		// check for "Re"
		if (p > stopper - 3)
			break;
		if ((*p == 'R' || *p == 'r') && (p[1] == 'e' || p[1] == 'E') && p[2] == ':')
			p += 3;
		else
			break;
		}
	return string_slice(p, subject.end());
}


time_t Newsgroup::SliceToDate(string_slice strIn)
{
	char str[256];
	strIn.copy(str);
	str[strIn.length()] = 0;
	return parsedate(str, -1);
}


void Newsgroup::CalcNumUnreadArticles()
{
	numUnreadArticles = 0;

	// count intersection of available and unread articles
	IntRange lastReadRange(-1, -1);
	int numRanges = readArticles->NumRanges();
	for (int i=0; i<numRanges; i++) {
		IntRange readRange = readArticles->RangeAt(i);

		// unread range is between readRanges
		IntRange unreadRange(lastReadRange.max + 1, readRange.min - 1);

		// add intersection
		IntRange unreadAvailable = unreadRange & availableArticles;
		if (unreadAvailable.IsValid())
			numUnreadArticles += unreadAvailable.max - unreadAvailable.min + 1;

		lastReadRange = readRange;
		}
	// handle last unread range
	IntRange lastUnreadRange(lastReadRange.max + 1, availableArticles.max);
	IntRange lastUnreadAvailable = lastUnreadRange & availableArticles;
	if (lastUnreadAvailable.IsValid())
		numUnreadArticles += lastUnreadAvailable.max - lastUnreadAvailable.min + 1;

	// display
	newsrc->NewsgroupChanged(this);
}


BFile* Newsgroup::OpenHeaderCacheFile(uint32 openMode)
{
	status_t err;

	// figure out where to put it
	BPath path;
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (err != B_NO_ERROR)
		return NULL;
	path.Append(FileNames::cacheFolderName);
	err = create_directory(path.Path(), 0777);
	if (err != B_NO_ERROR)
		return NULL;
	path.Append(qstring(groupName).c_str());

	// open the file
	return new BFile(path.Path(), openMode);
}


void Newsgroup::WriteSubject(Subject* subject, void* data)
{
	// write the subject
	string_slice subj = subject->GetSubject();
	fwrite(subj.begin(), subj.length(), 1, stdout);
	printf("\n");

	// write the articles
	int numArticles = subject->NumArticles();
	for (int i=0; i<numArticles; i++) {
		Article* article = subject->ArticleAt(i);
		printf("\t%d\t", article->ArticleNo());
		string_slice author = article->Author();
		fwrite(author.begin(), author.length(), 1, stdout);
		printf("\t%d\n", article->Lines());
		}
}


