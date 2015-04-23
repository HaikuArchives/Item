/* Thread.h */

#ifndef _H_Thread_
#define _H_Thread_

#include <OS.h>

class Thread {
public:
	Thread(const char* name = NULL, int32 priority = B_NORMAL_PRIORITY);
	virtual~Thread();
	virtual void	Action();
	status_t    	Go();
	void        	WaitFor();

protected:
	static int32	ThreadStarter(void* data);

	thread_id	threadID;
};


#endif
