/* GetNewGroupsThread.h */

#ifndef _H_GetNewGroupsThread_
#define _H_GetNewGroupsThread_

#include "OwnedThread.h"

class GetNewGroupsThread : public OwnedThread {
public:
	GetNewGroupsThread();
	void	Action();
};


#endif
