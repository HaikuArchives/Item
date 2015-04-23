/* PostArticlesThread.h */

#ifndef _H_PostArticlesThread_
#define _H_PostArticlesThread_

#include "OwnedThread.h"

class NNTPConnection;
class Task;
class qstring;

class PostArticlesThread : public OwnedThread {
public:
	PostArticlesThread(BList* fileRefsIn);
	~PostArticlesThread();
	void    	Action();
	status_t	PostArticle(entry_ref* fileRef, NNTPConnection* connection, Task* task);
	qstring*	GetFromHeader();
	qstring*	GetReplyToHeader();
	qstring*	GetMessageIDHeader(NNTPConnection* connection);

protected:
	BList*	fileRefs;
};


#endif
