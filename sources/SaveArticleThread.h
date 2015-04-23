/* SaveArticleThread.h */

#ifndef _H_SaveArticleThread_
#define _H_SaveArticleThread_

#include "ArticleSavingThread.h"
#include "string_slice.h"
#include "qstring.h"
#include "LineEndType.h"
#include <Message.h>

class NNTPResponse;
class NNTPConnection;
class Task;

#define copies

class SaveArticleThread : public ArticleSavingThread {
public:
	SaveArticleThread(copies BMessage* messageIn);
	void       	Action();
	void       	SaveMessage(entry_ref* fileRef, string_slice groupName, int articleNo, string_slice subject);
	LineEndType	GetLineEndType(BFile* file);

protected:
	BMessage                 	message;
	static const string_slice	divider;
};


#endif
