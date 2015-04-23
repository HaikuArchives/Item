/* Thread.cpp */

#include "Thread.h"



Thread::Thread(const char* name, int32 priority)
{
	if (name == NULL)
		name = "";
	threadID = spawn_thread(ThreadStarter, name, priority, this);
}


Thread::~Thread()
{
	// just need to make this virtual
}


void Thread::Action()
{
	/** pure virtual **/
}


status_t Thread::Go()
{
	return resume_thread(threadID);
}


void Thread::WaitFor()
{
	status_t exitValue;
	wait_for_thread(threadID, &exitValue);
}


int32 Thread::ThreadStarter(void* data)
{
	Thread* thread = (Thread*) data;
	thread->Action();
	delete thread;
	return 0;
}


