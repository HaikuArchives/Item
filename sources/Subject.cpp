/* Subject.cpp */

#include "Subject.h"
#include "Article.h"
#include "Newsgroup.h"
#include "ExtractBinariesThread.h"
#include <Message.h>



Subject::Subject(string_slice subjectIn)
	: subject(subjectIn), filterScore(0), minArticleNo(0),
	  expanded(false), selected(false)
{
}


void Subject::AddArticle(Article* article)
{
	articles.AddItem(article);

	// update "filterValue" and "minArticleNo"
	if (articles.CountItems() == 1) {
		filterScore = article->FilterScore();
		minArticleNo = article->ArticleNo();
		minArticleDate = article->Date();
		}
	else {
		float newFilterScore = article->FilterScore();
		if (newFilterScore > filterScore)
			filterScore = newFilterScore;
		int articleNo = article->ArticleNo();
		if (articleNo < minArticleNo)
			minArticleNo = articleNo;
		time_t date = article->Date();
		if (date < minArticleDate)
			minArticleDate = date;
		}
}


void Subject::RemoveArticle(Article* article)
{
	articles.RemoveItem(article);

	// update "filterValue", "minArticleNo", "minArticleDate"
	Article* curArticle;
	int numArticles = NumArticles();
	if (numArticles == 0) {
		filterScore = 0;
		minArticleNo = 0;
		minArticleDate = 0;
		}
	else {
		curArticle = ArticleAt(0);
		filterScore = curArticle->FilterScore();
		minArticleNo = curArticle->ArticleNo();
		minArticleDate = curArticle->Date();
		}
	for (int i=1; i<numArticles; i++) {
		curArticle = ArticleAt(i);
		float newFilterScore = curArticle->FilterScore();
		if (newFilterScore > filterScore)
			newFilterScore = filterScore;
		int articleNo = curArticle->ArticleNo();
		if (articleNo < minArticleNo)
			minArticleNo = articleNo;
		time_t date = curArticle->Date();
		if (date < minArticleDate)
			minArticleDate = date;
		}
}


void Subject::SetupSaveArticles(BMessage* message)
{
	qstring subjectStr(subject);

	// add selected articles
	int numArticles = NumArticles();
	bool hadSelectedArticle = false;
	for (int i=0; i<numArticles; i++) {
		Article* article = ArticleAt(i);
		if (!article->Selected() && !selected)
			continue;
		message->AddInt32("articleNos", article->ArticleNo());
		message->AddString("subjects", subjectStr.c_str());
		hadSelectedArticle = true;
		}
}


void Subject::SetupExtractBinaries(BList* list)
{
	// add selected articles
	int numArticles = NumArticles();
	for (int i=0; i<numArticles; i++) {
		Article* article = ArticleAt(i);
		if (!article->Selected() && !selected)
			continue;
		BinaryArticleSpec* newSpec = new BinaryArticleSpec(this, article);
		list->AddItem(newSpec);
		}
}


string_slice Subject::GetSubject()
{
	return subject;
}


int Subject::NumArticles()
{
	return articles.CountItems();
}


int Subject::NumUnreadArticles()
{
	int numArticles = articles.CountItems();
	int numUnread = 0;
	for (int i=0; i<numArticles; i++) {
		Article* article = (Article*) articles.ItemAt(i);
		if (!article->IsRead())
			numUnread += 1;
		}
	return numUnread;
}


Article* Subject::ArticleAt(int index)
{
	return (Article*) articles.ItemAt(index);
}


Article* Subject::FirstUnreadArticle()
{
	int numArticles = articles.CountItems();
	for (int i=0; i<numArticles; i++) {
		Article* article = (Article*) articles.ItemAt(i);
		if (!article->IsRead())
			return article;
		}
	return NULL;
}


bool Subject::AllRead()
{
	int numArticles = articles.CountItems();
	for (int i=0; i<numArticles; i++) {
		Article* article = (Article*) articles.ItemAt(i);
		if (!article->IsRead())
			return false;
		}
	return true;
}


bool Subject::AllUnread()
{
	int numArticles = articles.CountItems();
	for (int i=0; i<numArticles; i++) {
		Article* article = (Article*) articles.ItemAt(i);
		if (article->IsRead())
			return false;
		}
	return true;
}


bool Subject::Expanded()
{
	return expanded;
}


void Subject::SetExpanded(bool newExpanded)
{
	expanded = newExpanded;
}


bool Subject::Selected()
{
	return selected;
}


void Subject::SetSelected(bool newSelected)
{
	selected = newSelected;
}


void Subject::MarkSelected(bool marked, Newsgroup* newsgroup)
{
	int i;
	Article* article;

	int numArticles = NumArticles();

	// if whole subject is selected, mark all articles
	if (selected) {
		for (i=0; i<numArticles; i++) {
			article = ArticleAt(i);
			article->SetRead(marked);
			if (marked)
				newsgroup->ArticleRead(article->ArticleNo());
			else
				newsgroup->ArticleUnread(article->ArticleNo());
			}
		}

	// otherwise, just mark selected articles
	else {
		for (i=0; i<numArticles; i++) {
			article = ArticleAt(i);
			if (article->Selected()) {
				article->SetRead(marked);
				if (marked)
					newsgroup->ArticleRead(article->ArticleNo());
				else
					newsgroup->ArticleUnread(article->ArticleNo());
				}
			}
		}
}


