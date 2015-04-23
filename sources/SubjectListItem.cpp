/* SubjectListItem.cpp */

#include "SubjectListItem.h"
#include "Subject.h"
#include "Article.h"
#include "Newsgroup.h"
#include "OpenArticleThread.h"
#include "qstring.h"

const float SubjectListItem::SubjectStart = 10;
const float SubjectListItem::MarkStart = 3;


SubjectListItem::SubjectListItem(Subject* subjectIn)
	: BaseListItem(false), subject(subjectIn)
{
}


void SubjectListItem::DrawItem(BView* view, BRect itemRect, bool drawEverything)
{
	SetupColors(view, itemRect);

	// draw the mark
	if (subject->AllRead())
		view->DrawString("√", BPoint(itemRect.left + MarkStart, itemRect.bottom - Baseline));
	else if (!subject->AllUnread())
		view->DrawString("–", BPoint(itemRect.left + MarkStart, itemRect.bottom - Baseline));

	// draw subject
	qstring str = subject->Subject();
	view->DrawString(str.c_str(),
	                 BPoint(itemRect.left + SubjectStart, itemRect.bottom - Baseline));
}


bool SubjectListItem::Open(Newsgroup* newsgroup)
{
	Article* article = subject->FirstUnreadArticle();
	if (article == NULL)
		article = subject->ArticleAt(0);
	article->SetRead(true);
	OpenArticleThread* thread = new OpenArticleThread(newsgroup, article->ArticleNo());
	thread->Go();
	return (subject->FirstUnreadArticle() == NULL);
}


void SubjectListItem::Mark(Newsgroup* newsgroup)
{
	int numArticles = subject->NumArticles();
	for (int i=0; i<numArticles; i++) {
		Article* article = subject->ArticleAt(i);
		article->SetRead(true);
		newsgroup->ArticleRead(article->ArticleNo());
		}
}


void SubjectListItem::Unmark(Newsgroup* newsgroup)
{
	int numArticles = subject->NumArticles();
	for (int i=0; i<numArticles; i++) {
		Article* article = subject->ArticleAt(i);
		article->SetRead(false);
		newsgroup->ArticleUnread(article->ArticleNo());
		}
}


