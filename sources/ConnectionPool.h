/* ConnectionPool.h */

#ifndef _H_ConnectionPool_
#define _H_ConnectionPool_

#include <List.h>
#include <Locker.h>

class NNTPConnection;

class ConnectionPool {
public:
	static ConnectionPool*	GetPool();
	static bool           	HavePool();
	static void           	DeletePool();

protected:
	ConnectionPool();

public:
	~ConnectionPool();
	NNTPConnection*       	GetConnection();
	void                  	ReleaseConnection(NNTPConnection* connection);
	int                   	NumConnections();
	NNTPConnection*       	ConnectionAt(int whichConnection);
	int                   	MaxConnections();

protected:
	BList                 	connections;
	sem_id                	semaphore;
	BLocker               	lock;
	static ConnectionPool*	connectionPool;

public:
	static const int      	defaultMaxConnections;
};


#endif
