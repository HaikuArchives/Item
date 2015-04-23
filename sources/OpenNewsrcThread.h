/* OpenNewsrcThread.h */

#ifndef _H_OpenNewsrcThread_
#define _H_OpenNewsrcThread_

#include "Thread.h"
#include <Entry.h>

/*
	This really doesn't need to be a thread anymore, since the real multi-threaded
	functionality has been moved to GetGroupStatsThread.  But since it already is
	one, I'm not going to bother to get rid of it.
*/

class OpenNewsrcThread : public Thread {
public:
	OpenNewsrcThread(entry_ref* fileRefIn);
	void	Action();

protected:
	entry_ref	fileRef;
};


#endif
