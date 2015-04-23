/* GetGroupsListThread.h */

#ifndef _H_GetGroupsListThread_
#define _H_GetGroupsListThread_

#include "OwnedThread.h"

class AllGroupsList;

class GetGroupsListThread : public OwnedThread {
public:
	GetGroupsListThread(AllGroupsList* groupsListIn);
	void	Action();

protected:
	AllGroupsList*  	groupsList;
	static const int	progressQuantum;
};


#endif
