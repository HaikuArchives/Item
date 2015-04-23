/* OpenArticleThread.h */

#ifndef _H_OpenArticleThread_
#define _H_OpenArticleThread_

#include "ArticleSavingThread.h"
#include "string_slice.h"

class Article;
class Newsgroup;
class NNTPResponse;
class NNTPConnection;
class Task;

class OpenArticleThread : public ArticleSavingThread {
public:
	OpenArticleThread(Newsgroup* newsgroupIn, int articleNoIn, string_slice subjectIn, int linesIn);
	void        	Action();
	string_slice	ExtractMessageID(NNTPResponse* response);

protected:
	Newsgroup*  	newsgroup;
	int         	articleNo;
	string_slice	subject;
	int         	lines;
};


#endif
