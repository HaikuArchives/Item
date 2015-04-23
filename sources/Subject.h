/* Subject.h */

#ifndef _H_Subject_
#define _H_Subject_

#include "string_slice.h"
#include <List.h>

class Article;
class Newsgroup;
class BMessage;

class Subject {
public:
	Subject(string_slice subjectIn);
	void         	AddArticle(Article* article);
	void         	RemoveArticle(Article* article);
	void         	SetupSaveArticles(BMessage* message);
	void         	SetupExtractBinaries(BList* list);
	string_slice 	GetSubject();
	inline float 	FilterScore();
	inline int   	MinArticleNo();
	inline time_t	MinArticleDate();
	int          	NumArticles();
	int          	NumUnreadArticles();
	Article*     	ArticleAt(int index);
	Article*     	FirstUnreadArticle();
	bool         	AllRead();
	bool         	AllUnread();
	bool         	Expanded();
	void         	SetExpanded(bool newExpanded);
	bool         	Selected();
	void         	SetSelected(bool newSelected);
	void         	MarkSelected(bool marked, Newsgroup* newsgroup);

protected:
	string_slice	subject;
	BList       	articles;
	float       	filterScore;
	int         	minArticleNo;
	time_t      	minArticleDate;
	bool        	expanded;
	bool        	selected;
};

inline float Subject::FilterScore()
{
	return filterScore;
}



inline int Subject::MinArticleNo()
{
	return minArticleNo;
}



inline time_t Subject::MinArticleDate()
{
	return minArticleDate;
}




#endif
