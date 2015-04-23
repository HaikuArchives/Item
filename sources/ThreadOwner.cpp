/* ThreadOwner.cpp */

#include "ThreadOwner.h"
#include "OwnedThread.h"
#include <List.h>



ThreadOwner::ThreadOwner()
{
	threads = new BList();
}


ThreadOwner::~ThreadOwner()
{
	KillOwnedThreads();
}


void ThreadOwner::StartThread(OwnedThread* thread)
{
	threadsLock.Lock();
	threads->AddItem(thread);
	threadsLock.Unlock();
	thread->SetOwner(this);
	thread->Go();
}


void ThreadOwner::KillOwnedThreads()
{
	// signal all threads to stop
	threadsLock.Lock();
	int numThreads = threads->CountItems();
	for (int i=0; i<numThreads; i++) {
		OwnedThread* thread = (OwnedThread*) threads->ItemAt(i);
		thread->Stop();
		}
	threadsLock.Unlock();

	while (true) {
		// get the next thread, if there are any left
		threadsLock.Lock();
		OwnedThread* thread = (OwnedThread*) threads->ItemAt(0);
		threadsLock.Unlock();
		if (thread == NULL)
			break;

		// wait for it to die
		//*** POSSIBLE RACE CONDITION--if thread dies after the Unlock() but before
		//*** we get here.
		thread->WaitFor();
		}
}


void ThreadOwner::ThreadFinished(OwnedThread* thread)
{
	threadsLock.Lock();
	threads->RemoveItem(thread);
	threadsLock.Unlock();
}


