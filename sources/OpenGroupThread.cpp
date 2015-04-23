/* OpenGroupThread.cpp */

#include "OpenGroupThread.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "ConnectionPool.h"
#include "NewsgroupWind.h"
#include "Task.h"
#include "qstring.h"
#include "Error.h"

class ArticleSet;



OpenGroupThread::OpenGroupThread(Newsgroup* newsgroupIn)
	: OwnedThread("Open Group"), newsgroup(newsgroupIn)
{
}


void OpenGroupThread::Action()
{
	NNTPResponse* response;
	int result;

	Task task(newsgroup->Name());

	// open the window
	new NewsgroupWind(newsgroup);
	snooze(20000);	// this seems to be the only way to get the window to display
	              	// before ReadArticleCache() locks it

	// read the cached headers, even before getting a connection, since that
	// could take a few seconds
	ArticleSet* cachedArticles = newsgroup->ReadArticleCache();

	// get a connection
	ConnectionPool* connectionPool = ConnectionPool::GetPool();
	NNTPConnection* connection = connectionPool->GetConnection();
	if (connection == NULL) {
		Error::ShowError("Couldn't get a connection to read ", newsgroup->Name(), "!");
		return;
		}
	task.GotConnection();
	task.SetProgressString(newsgroup->Name());

	// go to the newsgroup and get the stats if necessary
	//    no, do it always; unless we tell the newsgroup there are more
	//    are more articles available, it won't know it has to get
	//    more headers
	const bool needStats = true;
/***
	bool needStats = (!newsgroup->HaveAvailableArticlesInfo() ||
	                  connection->CurrentGroup() != newsgroup->Name());
***/
	if (needStats) {
		qstring cmd = "group ";
		cmd += newsgroup->Name();
		connection->SendCommand(cmd);
		}

	// get the results of the "group" command if we sent one
	if (needStats) {
		result = connection->GetResponse(&response);
		if (result != 211) {
			if (result == 411)
				Error::ShowError(newsgroup->Name(), " no longer exists on the server.");
			else
				Error::ShowError("Couldn't open ", newsgroup->Name(), ".", result);
			connectionPool->ReleaseConnection(connection);
			return;
			}
		connection->SetGroup(newsgroup->Name());
		int numArticles = response->NextIntField();	// ignore "unused variable" warning
		int firstArticle = response->NextIntField();
		int lastArticle = response->NextIntField();
		delete response;
		newsgroup->SetAvailableArticles(firstArticle, lastArticle);
		}
	if (stopRequested)
		goto stop;

	// have the newsgroup read its headers
	result = newsgroup->ReadHeaders(connection, cachedArticles, &task, stopRequested);
	if (result != B_NO_ERROR) {
		Error::ShowError("Couldn't get headers for ", newsgroup->Name(), ".", result);
		delete newsgroup;
		connectionPool->ReleaseConnection(connection);
		return;
		}
	if (stopRequested)
		goto stop;

stop:
	// clean up
	connectionPool->ReleaseConnection(connection);
}


