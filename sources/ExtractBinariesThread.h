/* ExtractBinariesThread.h */

#ifndef _H_ExtractBinariesThread_
#define _H_ExtractBinariesThread_

#include "ArticleSavingThread.h"

class Newsgroup;
class Subject;
class Article;
class SubjectsView;
class BList;

struct BinaryArticleSpec {
	Subject*	subject;
	Article*	article;

	BinaryArticleSpec(Subject* subjectIn, Article* articleIn)
		: subject(subjectIn), article(articleIn) {}
};

#define takes

class ExtractBinariesThread : public ArticleSavingThread {
public:
	ExtractBinariesThread(Newsgroup* newsgroupIn, takes BList* articlesIn,
                      SubjectsView* subjectsViewIn, bool multipartIn);
	void	Action();
	void	ExtractMultipleBinaries();
	void	ExtractMultipartBinary();
	void	GetTempFileName(char* tempFileName);

protected:
	Newsgroup*   	newsgroup;
	BList*       	articles;
	SubjectsView*	subjectsView;
	bool         	multipart;
};


#endif
