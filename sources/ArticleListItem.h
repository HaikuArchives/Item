/* ArticleListItem.h */

#ifndef _H_ArticleListItem_
#define _H_ArticleListItem_

#include "BaseListItem.h"

class Article;

class ArticleListItem : public BaseListItem {
public:
	ArticleListItem(Article* articleIn);
	void	DrawItem(BView* view, BRect itemRect, bool drawEverything);
	bool	Open(Newsgroup* newsgroup);
	void	Mark(Newsgroup* newsgroup);
	void	Unmark(Newsgroup* newsgroup);

protected:
	Article*          	article;
	static const float	SubjectStart;
	static const float	MarkStart;
};


#endif
