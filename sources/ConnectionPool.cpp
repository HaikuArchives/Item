/* ConnectionPool.cpp */

#include "ConnectionPool.h"
#include "NNTPConnection.h"
#include "Messages.h"
#include "Prefs.h"
#include "Error.h"
#include <Autolock.h>

struct ConnectionSpec {
	NNTPConnection*	connection;
	bool           	inUse;

	ConnectionSpec(NNTPConnection* connectionIn)
		: connection(connectionIn), inUse(false) {}
};

ConnectionPool* ConnectionPool::connectionPool = NULL;
const int ConnectionPool::defaultMaxConnections = 4;


ConnectionPool* ConnectionPool::GetPool()
{
	if (connectionPool == NULL)
		connectionPool = new ConnectionPool();
	return connectionPool;
}


bool ConnectionPool::HavePool()
{
	return (connectionPool != NULL);
}


void ConnectionPool::DeletePool()
{
	if (connectionPool) {
		delete connectionPool;
		connectionPool = NULL;
		}
}


ConnectionPool::ConnectionPool()
{
	semaphore = create_sem(MaxConnections(), "NNTP Connections");
}


ConnectionPool::~ConnectionPool()
{
	// ASSUMES ALL CONNECTIONS HAVE BEEN RELEASED
	// (which could be remedied by "acquire_sem_etc(semaphore, MaxConnections())")

	// close and delete all connections
	int numConnections = connections.CountItems();
	for (int i=0; i<numConnections; i++) {
		ConnectionSpec* info = (ConnectionSpec*) connections.ItemAt(i);
		NNTPConnection* connection = info->connection;
		connection->SendCommand("quit");
		delete connection;
		delete info;
		}
	connections.MakeEmpty();

	delete_sem(semaphore);
}


NNTPConnection* ConnectionPool::GetConnection()
{
	// block until there's a free connection
	if (acquire_sem(semaphore) != B_NO_ERROR)
		return NULL;

	if (!BAutolock(lock).IsLocked())
		return NULL;

	// find a free connection
	int numConnections = connections.CountItems();
	for (int i=0; i<numConnections; i++) {
		ConnectionSpec* info = (ConnectionSpec*) connections.ItemAt(i);
		if (!info->inUse) {
			// found it!  return it.
			info->inUse = true;
			return info->connection;
			}
		}

	// no free connection; add one
	// create the connection
	NNTPConnection* connection = new NNTPConnection();
	ConnectionSpec* newInfo = new ConnectionSpec(connection);
	connections.AddItem(newInfo);
	newInfo->inUse = true;
	// connect
	status_t err = connection->Connect();
	if (err != B_NO_ERROR) {
		Error::ShowError("Couldn't open connection.", err);
		connections.RemoveItem(newInfo);
		delete newInfo;
		delete connection;
		return NULL;
		}
	return connection;
}


void ConnectionPool::ReleaseConnection(NNTPConnection* connection)
{
	if (!BAutolock(lock).IsLocked())
		return;

	int numConnections = connections.CountItems();
	for (int i=0; i<numConnections; i++) {
		ConnectionSpec* info = (ConnectionSpec*) connections.ItemAt(i);
		if (info->connection == connection) {
			// found it!  release it
			info->inUse = false;
			release_sem(semaphore);
			return;
			}
		}
}


int ConnectionPool::NumConnections()
{
	if (!BAutolock(lock).IsLocked())
		return 0;

	int connectionsInUse = 0;
	int numConnections = connections.CountItems();
	for (int i=0; i<numConnections; i++) {
		ConnectionSpec* info = (ConnectionSpec*) connections.ItemAt(i);
		if (info->inUse)
			connectionsInUse++;
		}

	return connectionsInUse;
}


NNTPConnection* ConnectionPool::ConnectionAt(int whichConnection)
{
	if (!BAutolock(lock).IsLocked())
		return 0;

	int connectionsInUse = 0;
	int numConnections = connections.CountItems();
	for (int i=0; i<numConnections; i++) {
		ConnectionSpec* info = (ConnectionSpec*) connections.ItemAt(i);
		if (info->inUse) {
			if (whichConnection == connectionsInUse)
				return info->connection;
			connectionsInUse++;
			}
		}

	return NULL;
}


int ConnectionPool::MaxConnections()
{
	return (Prefs()->GetInt32Pref("maxConnections", defaultMaxConnections));
}


