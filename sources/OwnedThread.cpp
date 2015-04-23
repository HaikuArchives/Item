/* OwnedThread.cpp */

#include "OwnedThread.h"
#include "ThreadOwner.h"



OwnedThread::OwnedThread(const char* name, int32 priority)
	: Thread(name, priority), threadOwner(NULL), stopRequested(false)
{
}


OwnedThread::~OwnedThread()
{
	if (threadOwner)
		threadOwner->ThreadFinished(this);
}


void OwnedThread::Stop()
{
	// virtual; default sets flag, but subclasses are free to use whatever
	// mechanism is appropriate
	stopRequested = true;
}


void OwnedThread::SetOwner(ThreadOwner* owner)
{
	threadOwner = owner;
}


