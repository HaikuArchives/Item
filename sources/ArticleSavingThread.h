/* ArticleSavingThread.h */

#ifndef _H_ArticleSavingThread_
#define _H_ArticleSavingThread_

#include "Thread.h"
#include "string_slice.h"

class BFile;
class NNTPResponse;
class Task;

class ArticleSavingThread : public Thread {
public:
	ArticleSavingThread(const char* name = NULL, int32 priority = B_NORMAL_PRIORITY);
	status_t	WriteFile(BFile* file, NNTPResponse* response, string_slice lineEnd,
                   Task* task, bool forBinary = false);
};


#endif
