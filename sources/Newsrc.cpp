/* Newsrc.cpp */

#include "Newsrc.h"
#include "Newsgroup.h"
#include "ArticleSet.h"
#include "NewsgroupsView.h"
#include <stdio.h>
#include <File.h>
#include <List.h>
#include <Window.h>



Newsrc::Newsrc(BFile* fileIn)	// takes ownership of "file"
	: file(fileIn)
{
	newsgroups = new BList();
}


Newsrc::~Newsrc()
{
	Save();

	delete file;

	// delete the newsgroups
	int numGroups = newsgroups->CountItems();
	for (int i=0; i<numGroups; i++)
		delete (Newsgroup*) newsgroups->ItemAt(i);
	delete newsgroups;
}


void Newsrc::Read()
{
	// sanity clause
	if (file == NULL || file->InitCheck() != B_NO_ERROR)
		return;

	// read the file into memory
	off_t fileSize;
	file->GetSize(&fileSize);
	char* fileText = new char[fileSize];
	file->ReadAt(0, fileText, fileSize);

	// read the file, line-by-line
	const char* p = fileText;
	const char* stopper = fileText + fileSize;
	while (p < stopper) {
		// get the line
		const char* lineStart = p;
		while (p < stopper && *p != '\n' && *p != '\r')
			p++;
		string_slice line(lineStart, p);
		if (p < stopper)
			p++;	// skip EOL
		// skip empty lines
		if (line.length() <= 0)
			continue;

		// parse the line
		string_slice groupName, articleSetStr;
		ParseLine(line, &groupName, &articleSetStr);
		if (groupName.length() <= 0)
			continue;

		// add the newsgroup
		Newsgroup* newsgroup = new Newsgroup(groupName, this);
		if (articleSetStr.length() > 0) {
			ArticleSet* readArticles = new ArticleSet();
			readArticles->AddArticles(articleSetStr);
			newsgroup->SetReadArticles(readArticles);
			}
		newsgroups->AddItem(newsgroup);
		}

	// clean up
	delete fileText;
}


void Newsrc::Save()
{
	file->Seek(0, SEEK_SET);
	string_slice separator = ": ";
	string_slice lineEnd = "\r";	// for Mac-compatibility

	// write out the newsgroups
	int numNewsgroups = newsgroups->CountItems();
	for (int i=0; i<numNewsgroups; i++) {
		Newsgroup* newsgroup = (Newsgroup*) newsgroups->ItemAt(i);

		// name
		string_slice name = newsgroup->Name();
		file->Write(name.begin(), name.length());

		// separator
		file->Write(separator.begin(), separator.length());

		// articles
		ArticleSet* readArticles = newsgroup->ReadArticles();
		qstring* readArticlesStr = readArticles->AsString();
		file->Write(readArticlesStr->data(), readArticlesStr->length());
		delete readArticlesStr;

		// EOL
		file->Write(lineEnd.begin(), lineEnd.length());
		}

	// finish the file
	file->SetSize(file->Position());
}


void Newsrc::NewsgroupChanged(Newsgroup* newsgroup)
{
	int index = newsgroups->IndexOf(newsgroup);
	if (index >= 0 && view) {
		BWindow* wind = view->Window();
		if (!wind->IsHidden() && wind->Lock()) {
			view->InvalidateItem(index);
			wind->Unlock();
			}
		}
}


void Newsrc::AttachedToView(NewsgroupsView* viewIn)
{
	view = viewIn;
}


int Newsrc::NumNewsgroups()
{
	return newsgroups->CountItems();
}


Newsgroup* Newsrc::NewsgroupAt(int index)
{
	return (Newsgroup*) newsgroups->ItemAt(index);
}


Newsgroup* Newsrc::RemoveNewsgroupAt(int index)
{
	Newsgroup* newsgroup = (Newsgroup*) newsgroups->RemoveItem(index);
	newsgroup->SetNewsrc(NULL);
	return newsgroup;
}


void Newsrc::AddNewsgroupAt(Newsgroup* newsgroup, int index)
{
	newsgroups->AddItem(newsgroup, index);
	newsgroup->SetNewsrc(this);
}


Newsgroup* Newsrc::NewsgroupNamed(string_slice groupName)
{
	int numGroups = newsgroups->CountItems();
	for (int i=0; i<numGroups; i++) {
		Newsgroup* group = (Newsgroup*) newsgroups->ItemAt(i);
		if (groupName == group->Name())
			return group;
		}
	return NULL;
}


void Newsrc::ParseLine(string_slice line, string_slice* groupName, string_slice* articleSet)
{
	*articleSet = string_slice();	// until we find it

	// find the group name
	const char* p = line.begin();
	const char* stopper = line.end();
	bool foundGroupName = false;
	for (; p < stopper; p++) {
		char c = *p;
		if (c == ' ' || c == '\t') {
			// not a real newsrc line
			break;
			}
		if (c == ':') {
			// found the end of the name
			*groupName = string_slice(line.begin(), p);
			foundGroupName = true;
			break;
			}
		}
	if (!foundGroupName) {
		*groupName = string_slice();
		return;
		}
	p++;	// skip ':'

	// skip whitespace
	while (p < stopper && (*p == ' ' || *p == '\t'))
		p++;

	// trim trailing whitespace and set up the articleSet
	const char* articleSetStart = p;
	p = stopper - 1;
	for (; p >= articleSetStart; --p) {
		char c = *p;
		if (c != ' ' || c != '\t')
			break;
		}
	*articleSet = string_slice(articleSetStart, p + 1);
}


