/* ThreadOwner.h */

#ifndef _H_ThreadOwner_
#define _H_ThreadOwner_

#include <Locker.h>

class OwnedThread;
class BList;

/* mix-in class for objects that need to own threads */

class ThreadOwner {
public:
	ThreadOwner();
	~ThreadOwner();
	void	StartThread(OwnedThread* thread);
	void	KillOwnedThreads();
	void	ThreadFinished(OwnedThread* thread);

protected:
	BList* 	threads;
	BLocker	threadsLock;
};


#endif
