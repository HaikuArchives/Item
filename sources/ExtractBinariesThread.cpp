/* ExtractBinariesThread.cpp */

#include "ExtractBinariesThread.h"
#include "Newsgroup.h"
#include "Subject.h"
#include "Article.h"
#include "ConnectionGetter.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "SubjectsView.h"
#include "Task.h"
#include "Preferences.h"
#include "Error.h"
#include "OSException.h"
#include <File.h>
#include <Entry.h>
#include <List.h>
#include <stdio.h>
#include <unistd.h>
#include <memory>



ExtractBinariesThread::ExtractBinariesThread(Newsgroup* newsgroupIn, takes BList* articlesIn, SubjectsView* subjectsViewIn, bool multipartIn)
	: ArticleSavingThread("Extract Binaries"),
	  newsgroup(newsgroupIn), articles(articlesIn), subjectsView(subjectsViewIn),
	  multipart(multipartIn)
{
}


void ExtractBinariesThread::Action()
{
	if (multipart)
		ExtractMultipartBinary();
	else
		ExtractMultipleBinaries();
}


void ExtractBinariesThread::ExtractMultipleBinaries()
{
	int i;
	BinaryArticleSpec* spec;
	NNTPResponse* response = NULL;

	try {

	Task task("Extracting binaries...");

	// set up the dirPath
	qstring dirPath = Prefs()->GetStringPref("binariesPath");
	if (dirPath.empty())
		throw OSException("You must specify a path for binaries in the Preferences before trying to extract binaries.");
	if (dirPath.data()[dirPath.length() - 1] != '/')
		dirPath += '/';

	// get the total number of lines
	int numArticles = articles->CountItems();
	unsigned int totalNumLines = 0;
	for (i=0; i<numArticles; ++i) {
		spec = (BinaryArticleSpec*) articles->ItemAt(i);
		totalNumLines += spec->article->Lines();
		}
	task.SetProgressMax(totalNumLines);

	// extract each file one by one
	bool undecodedBinaries = false;
	for (i=0; i<numArticles; ++i) {
		spec = (BinaryArticleSpec*) articles->ItemAt(i);
		task.SetProgressString(spec->subject->GetSubject());

		// get a connection
		ConnectionGetter connectionGetter;
		NNTPConnection* connection = connectionGetter.Connection();
		connection->SelectGroup(newsgroup->Name());
		task.GotConnection();

		// open the file
		qstring filePath = dirPath;
		char tempFileName[64];
		GetTempFileName(tempFileName);
		filePath += tempFileName;
		BFile file(filePath.c_str(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		status_t result = file.InitCheck();
		if (result != B_NO_ERROR)
			throw OSException("Couldn't create a file to extract binaries to.", result);

		// get the article
		int articleNo = spec->article->ArticleNo();
		qstring cmd = "article ";
		cmd += articleNo;
		result = connection->SendCommand(cmd);
		if (result == B_NO_ERROR)
			result = connection->GetResponse(&response);
		if (result != 220)
			throw OSException("Couldn't get article.", result);

		// write it to the file
		result = WriteFile(&file, response, "\n", &task, true);
		if (result < B_NO_ERROR)
			throw OSException("An error occurred while extracting binaries.", result);

		// mark as read in the newsgroup
		newsgroup->ArticleRead(articleNo);
		newsgroup->ReadArticlesChanged();
		spec->article->SetRead(true);
		subjectsView->InvalidateSubject(spec->subject);

		// clean up
		delete response;
		response = NULL;
		file.Unset();	// closes the file

		// decode the file
		task.SetProgressString("Decoding binary...");
		result = chdir(dirPath.c_str());
		if (result != 0)
			throw OSException("Couldn't decode the binary [error setting dir].", result);
		qstring command = "/bin/uudecode \"";
		command += filePath;
		command += "\"";
		result = system(command.c_str());
		if (result != 0)
			undecodedBinaries = true;

		// delete the file
		BEntry fileEntry(filePath.c_str());
		fileEntry.Remove();

		task.ReleasedConnection();
		}

	// warn about undecoded binaries
	if (undecodedBinaries)
		Error::ShowError("One or more binaries couldn't be decoded and have been left in their encoded form.");
	}
	catch (DisplayableException& e) {
		delete response;
		e.Display();
		}
}


void ExtractBinariesThread::ExtractMultipartBinary()
{
	int i;
	BinaryArticleSpec* spec;
	NNTPResponse* response = NULL;

	try {

	Task task("Extracting binaries...");

	// open the file
	qstring dirPath = Prefs()->GetStringPref("binariesPath");
	if (dirPath.empty())
		throw OSException("You must specify a path for binaries in the Preferences before trying to extract binaries.");
	qstring filePath = dirPath;
	if (filePath.data()[filePath.length() - 1] != '/')
		filePath += '/';
	char tempFileName[64];
	GetTempFileName(tempFileName);
	filePath += tempFileName;
	BFile file(filePath.c_str(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	status_t result = file.InitCheck();
	if (result != B_NO_ERROR)
		throw OSException("Couldn't create a file to extract binaries to.", result);

	// get the total number of lines
	int numArticles = articles->CountItems();
	unsigned int totalNumLines = 0;
	for (i=0; i<numArticles; ++i) {
		spec = (BinaryArticleSpec*) articles->ItemAt(i);
		totalNumLines += spec->article->Lines();
		}
	task.SetProgressMax(totalNumLines);

	// read all the articles into the file
	for (i=0; i<numArticles; ++i) {
		spec = (BinaryArticleSpec*) articles->ItemAt(i);
		task.SetProgressString(spec->subject->GetSubject());

		// get a connection
		ConnectionGetter connectionGetter;
		NNTPConnection* connection = connectionGetter.Connection();
		connection->SelectGroup(newsgroup->Name());
		task.GotConnection();

		// get the article
		int articleNo = spec->article->ArticleNo();
		qstring cmd = "article ";
		cmd += articleNo;
		result = connection->SendCommand(cmd);
		if (result == B_NO_ERROR)
			result = connection->GetResponse(&response);
		if (result != 220)
			throw OSException("Couldn't get article.", result);

		// append it to the file
		result = WriteFile(&file, response, "\n", &task, true);
		if (result < B_NO_ERROR)
			throw OSException("An error occurred while extracting binaries.", result);

		// mark as read in the newsgroup
		newsgroup->ArticleRead(articleNo);
		newsgroup->ReadArticlesChanged();
		spec->article->SetRead(true);
		subjectsView->InvalidateSubject(spec->subject);

		// clean up
		delete response;
		response = NULL;
		task.ReleasedConnection();
		}
	file.Unset();	// closes the file

	// decode the file
	task.SetProgressString("Decoding binary...");
	chdir(dirPath.c_str());
	qstring command = "/bin/uudecode \"";
	command += filePath;
	command += "\"";
	system(command.c_str());

	// delete the file
	BEntry fileEntry(filePath.c_str());
	fileEntry.Remove();

	}
	catch (DisplayableException& e) {
		delete response;
		e.Display();
		}
}


void ExtractBinariesThread::GetTempFileName(char* tempFileName)
{
	unsigned int microseconds = system_time() % 1000000;
	sprintf(tempFileName, "• %u%u.uu", (unsigned int) real_time_clock(), microseconds);
}


