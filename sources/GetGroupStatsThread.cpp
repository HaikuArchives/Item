/* GetGroupStatsThread.cpp */

#include "GetGroupStatsThread.h"
#include "Newsrc.h"
#include "NewsrcWind.h"
#include "Newsgroup.h"
#include "ConnectionPool.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "Task.h"
#include "Error.h"



GetGroupStatsThread::GetGroupStatsThread(Newsrc* newsrcIn, NewsrcWind* windIn)
	: OwnedThread("GetGroupStats"), newsrc(newsrcIn), wind(windIn)
{
}


void GetGroupStatsThread::Action()
{
	Task task("Getting article counts...");

	// get a connection
	ConnectionPool* connectionPool = ConnectionPool::GetPool();
	NNTPConnection* connection = connectionPool->GetConnection();
	if (connection == NULL) {
		Error::ShowError("Couldn't get a connection.");
		return;
		}
	task.GotConnection();

	// start getting the available article ranges
	NNTPResponse* response = NULL;
	int numGroups = newsrc->NumNewsgroups();
	task.SetProgressMax(numGroups);
	for (int i=0; i<numGroups; i++) {
		if (stopRequested)
			break;

		Newsgroup* newsgroup = newsrc->NewsgroupAt(i);
		task.SetProgress(i);
		task.SetProgressString(newsgroup->Name());

		// send the "group" command
		qstring cmd = "group ";
		cmd += newsgroup->Name();
		connection->SendCommand(cmd);

		// get the results
		int result = connection->GetResponse(&response);
		if (result != 211) {
			if (result == 411)
				continue;
			else {
				// probably something wrong with the connection
				// show an error?
				continue;	// break?
				}
			}
		connection->SetGroup(newsgroup->Name());
		int numArticles = response->NextIntField();		// ignore the "unused variable" warning.
		int firstArticle = response->NextIntField();
		int lastArticle = response->NextIntField();
		delete response;

		// update newsgroup & wind
		newsgroup->SetAvailableArticles(firstArticle, lastArticle);
		if (!stopRequested)
			wind->InvalidateNewsgroupAt(i);
		}

	connectionPool->ReleaseConnection(connection);
}


