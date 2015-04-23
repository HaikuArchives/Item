/* OwnedThread.h */

#ifndef _H_OwnedThread_
#define _H_OwnedThread_

#include "Thread.h"

class ThreadOwner;

class OwnedThread : public Thread {
public:
	OwnedThread(const char* name = NULL, int32 priority = B_NORMAL_PRIORITY);
	virtual~OwnedThread();
	virtual void	Stop();
	void        	SetOwner(ThreadOwner* owner);

protected:
	ThreadOwner*	threadOwner;
	bool        	stopRequested;
};


#endif
