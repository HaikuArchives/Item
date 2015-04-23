/* ConnectionGetter.h */

#ifndef _H_ConnectionGetter_
#define _H_ConnectionGetter_

#include "ConnectionPool.h"

class ConnectionGetter {
public:
	ConnectionGetter()
		: connection(ConnectionPool::GetPool()->GetConnection()) {}
	~ConnectionGetter()
		{ ConnectionPool::GetPool()->ReleaseConnection(connection); }

	NNTPConnection*	Connection() { return connection; }

protected:
	NNTPConnection*	connection;
};


#endif
