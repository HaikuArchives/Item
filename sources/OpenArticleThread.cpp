/* OpenArticleThread.cpp */

#include "OpenArticleThread.h"
#include "Newsgroup.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "ConnectionPool.h"
#include "ParseUtils.h"
#include "Task.h"
#include "Error.h"
#include <Path.h>
#include <FindDirectory.h>
#include <File.h>
#include <NodeInfo.h>
#include <Entry.h>
#include <Roster.h>



OpenArticleThread::OpenArticleThread(Newsgroup* newsgroupIn, int articleNoIn, string_slice subjectIn, int linesIn)
	: ArticleSavingThread("Open Article"), newsgroup(newsgroupIn), articleNo(articleNoIn),
	  subject(subjectIn), lines(linesIn)
{
}


void OpenArticleThread::Action()
{
	Task task(subject);
	task.SetProgressMax(lines);

	// get a connection
	ConnectionPool* connectionPool = ConnectionPool::GetPool();
	NNTPConnection* connection = connectionPool->GetConnection();
	if (connection == NULL) {
		Error::ShowError("Couldn't get a connection to read ", newsgroup->Name(), "!");
		return;
		}
	task.GotConnection();

	// select the newsgroup
	int result = connection->SelectGroup(newsgroup->Name());
	if (result != B_NO_ERROR) {
		Error::ShowError("Couldn't open ", newsgroup->Name(), " to get article.", result);
		connectionPool->ReleaseConnection(connection);
		return;
		}

	// get the article
	qstring cmd = "article ";
	cmd += articleNo;
	NNTPResponse* response = NULL;
	result = connection->SendCommand(cmd);
	if (result == B_NO_ERROR)
		result = connection->GetResponse(&response);
	if (result != 220) {
		Error::ShowError("Couldn't get article.", result);
		delete response;
		connectionPool->ReleaseConnection(connection);
		return;
		}

	// find the article's message-id--we'll use it as the filename
	string_slice messageID = ExtractMessageID(response);
	// make sure it doesn't have any slashes
	for (const char* p = messageID.begin(); p != messageID.end(); ++p) {
		if (*p == '/') {
			messageID = "xxx";
			break;
			}
		}

	// open the file
	BPath path;
	result = find_directory(B_SYSTEM_TEMP_DIRECTORY, &path, true);
	if (result != B_NO_ERROR) {
		Error::ShowError("Couldn't find temp directory.", result);
		delete response;
		connectionPool->ReleaseConnection(connection);
		return;
		}
	path.Append(qstring(messageID).c_str());
	BFile* file = new BFile(path.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	result = file->InitCheck();
	if (result != B_NO_ERROR) {
		Error::ShowError("Couldn't create message file.", result);
		delete response;
		connectionPool->ReleaseConnection(connection);
		delete file;
		return;
		}

	// set the MIME info
	BNodeInfo nodeInfo(file);
	nodeInfo.SetType("text/x-news");

	// write the file
	result = WriteFile(file, response, "\n", &task);
	delete file;
	delete response;
	connectionPool->ReleaseConnection(connection);
	if (result != B_NO_ERROR) {
		Error::ShowError("Error writing message file.", result);
		return;
		}

	// open the file with the viewer app
	entry_ref fileRef, appRef;
	result = get_ref_for_path(path.Path(), &fileRef);
	if (result == B_NO_ERROR)
		result = be_roster->FindApp(&fileRef, &appRef);
	if (result == B_NO_ERROR) {
		// cook up the B_REFS_RECEIVED message ourself so we can pass extra
		// info to the viewer app
		BMessage refsMessage(B_REFS_RECEIVED);
		refsMessage.AddRef("refs", &fileRef);
		refsMessage.AddBool("spacebarCanClose", true);
		be_roster->Launch(&appRef, &refsMessage);
		}

	// mark as read in the newsgroup
	newsgroup->ArticleRead(articleNo);
	newsgroup->ReadArticlesChanged();
}


string_slice OpenArticleThread::ExtractMessageID(NNTPResponse* response)
{
	string_slice name, value;
	while (true) {
		// get the next line
		string_slice line = response->NextLine();
		if (line.length() == 0) {
			// we're out of the headers, there was no message-id
			break;
			}

		// parse & check if it's the Message-ID header
		ParseUtils::ParseHeader(line, &name, &value);
		if (name == "Message-ID")
			return value;
		}

	/***/
	return string_slice("xxx");
}


