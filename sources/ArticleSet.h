/* ArticleSet.h */

#ifndef _H_ArticleSet_
#define _H_ArticleSet_

#include "IntRange.h"
#include "string_slice.h"

class IntRangeTree;
class qstring;

class ArticleSet {
public:
	ArticleSet();
	~ArticleSet();
	void        	AddArticle(int articleNo);
	void        	AddRange(IntRange range);
	void        	RemoveArticle(int articleNo);
	void        	RemoveRange(IntRange range);
	bool        	ContainsArticle(int articleNo);
	void        	AddArticles(string_slice str);
	qstring*    	AsString();
	void        	MakeEmpty();
	void        	Incorporate(ArticleSet* otherSet);
	unsigned int	NumRanges();
	IntRange    	RangeAt(unsigned int index);
	int         	NumArticles();
	int         	NumArticlesIn(IntRange inRange);

protected:
	IntRangeTree*	tree;
};


#endif
