/* OpenGroupThread.h */

#ifndef _H_OpenGroupThread_
#define _H_OpenGroupThread_

#include "OwnedThread.h"
#include "Newsgroup.h"

class OpenGroupThread : public OwnedThread {
public:
	OpenGroupThread(Newsgroup* newsgroupIn);
	void	Action();

protected:
	Newsgroup*	newsgroup;
};


#endif
