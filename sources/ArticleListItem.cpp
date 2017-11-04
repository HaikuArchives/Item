/* ArticleListItem.cpp */

#include "ArticleListItem.h"

#include <View.h>

#include "Article.h"
#include "Newsgroup.h"
#include "OpenArticleThread.h"
#include "qstring.h"

const float ArticleListItem::SubjectStart = 10;
const float ArticleListItem::MarkStart = 3;


ArticleListItem::ArticleListItem(Article* articleIn)
	: BaseListItem(true), article(articleIn)
{
}


void ArticleListItem::DrawItem(BView* view, BRect itemRect, bool drawEverything)
{
	SetupColors(view, itemRect);

	// draw the mark
	if (article->IsRead())
		view->DrawString("√", BPoint(itemRect.left + MarkStart, itemRect.bottom - Baseline));

	// draw author
	qstring str = article->Author();
	view->DrawString(str.c_str(),
	                 BPoint(itemRect.left + SubjectStart, itemRect.bottom - Baseline));
}


bool ArticleListItem::Open(Newsgroup* newsgroup)
{
	OpenArticleThread* thread = new OpenArticleThread(newsgroup, article->ArticleNo(), "", 0);
	thread->Go();
	article->SetRead(true);
	return true;
}


void ArticleListItem::Mark(Newsgroup* newsgroup)
{
	article->SetRead(true);
	newsgroup->ArticleRead(article->ArticleNo());
}


void ArticleListItem::Unmark(Newsgroup* newsgroup)
{
	article->SetRead(false);
	newsgroup->ArticleUnread(article->ArticleNo());
}


