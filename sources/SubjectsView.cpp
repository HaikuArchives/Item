/* SubjectsView.cpp */

#include "SubjectsView.h"
#include "Subject.h"
#include "SubjectTreeAdaptor.h"
#include "Article.h"
#include "OpenArticleThread.h"
#include "ListableTree.h"
#include "SubjectTree.h"
#include "NewsgroupWind.h"
#include "Messages.h"
#include <AppDefs.h>



SubjectsView::SubjectsView(BRect frame, ListableTree* tree, const char* name, uint32 resizingMode, uint32 flags)
	: AATreeListView(frame, tree, name, resizingMode, flags), selectDirection(None)
{
	SetViewColor(B_TRANSPARENT_32_BIT);

	SubjectTreeAdaptor::ExtraInfoDisplayChanged();	// intialize this
	if (SubjectTreeAdaptor::HasExtraInfo())
		SetFlags(Flags() | B_FULL_UPDATE_ON_RESIZE);
}


void SubjectsView::KeyDown(const char* bytes, int32 numBytes)
{
	int i, numSelections;
	SubjectTreeAdaptor* adaptor;

	switch (bytes[0]) {
		case B_RIGHT_ARROW:
			numSelections = NumSelections();
			for (i=0; i<numSelections; i++) {
				adaptor = (SubjectTreeAdaptor*) SelectionAt(i);
				adaptor->subject->SetExpanded(true);
				ItemDisplayChanged(adaptor);
				}
			break;

		case B_LEFT_ARROW:
			numSelections = NumSelections();
			for (i=0; i<numSelections; i++) {
				adaptor = (SubjectTreeAdaptor*) SelectionAt(i);
				adaptor->subject->SetExpanded(false);
				ItemDisplayChanged(adaptor);
				}
			break;

		case ' ':
			// "open next unread", much like MT-Newswatcher
			OpenNextUnread();
			break;

		case 'm':
		case 'M':
			Window()->PostMessage(MarkMessage);
			break;

		case 'u':
		case 'U':
			Window()->PostMessage(UnmarkMessage);
			break;

		case 'w':
		case 'W':
			Window()->PostMessage(B_QUIT_REQUESTED);
			break;

		case 'a':
		case 'A':
			Window()->PostMessage(B_SELECT_ALL);
			break;

		default:
			AATreeListView::KeyDown(bytes, numBytes);
			break;
		}
}


void SubjectsView::PrefChanged(string_slice prefName)
{
	if (prefName == "listBgndColor" || prefName == "listSelectedBgndColor" ||
			prefName == "listHilitedBgndColor" || prefName == "listTextColor") {
		Window()->Lock();
		Invalidate();
		Window()->Unlock();
		}
	else if (prefName == "listFont" || prefName == "extraInfoFont") {
		SubjectTreeAdaptor::FontChanged();
		tree->AllHeightsChanged();
		Window()->Lock();
		NumItemsChanged();
		Window()->Unlock();
		}
	else if (prefName == "showLines" || prefName == "showDates") {
		SubjectTreeAdaptor::ExtraInfoDisplayChanged();
		Window()->Lock();
			// it's not documented, but the window must be locked for a SetFlags() call
		if (SubjectTreeAdaptor::HasExtraInfo())
			SetFlags(Flags() | B_FULL_UPDATE_ON_RESIZE);
		else
			SetFlags(Flags() & ~B_FULL_UPDATE_ON_RESIZE);
		Invalidate();
		Window()->Unlock();
		}
}


SelectionDirection SubjectsView::SelectDirection()
{
	return selectDirection;
}


void SubjectsView::SetSelectDirection(SelectionDirection newSelectDirection)
{
	selectDirection = newSelectDirection;
}


void SubjectsView::InvalidateSubject(Subject* subject)
{
	Window()->Lock();
	InvalidateItem(((SubjectTree*) tree)->AdaptorFor(subject));
	Window()->Unlock();
}


void SubjectsView::OpenArticle(Article* article, Subject* subject)
{
	Newsgroup* newsgroup = ((NewsgroupWind*) Window())->GetNewsgroup();
	article->SetRead(true);
	InvalidateItem(((SubjectTree*) tree)->AdaptorFor(subject));
	OpenArticleThread* thread =
		new OpenArticleThread(newsgroup, article->ArticleNo(),
		                      subject->GetSubject(), article->Lines());
	thread->Go();
}


void SubjectsView::OpenNextUnread()
{
	SubjectTreeAdaptor* adaptor;
	Subject* subject;

	bool openedSomething = false;;
	int numSelections = NumSelections();
	for (int i=0; i<numSelections; i++) {
		adaptor = (SubjectTreeAdaptor*) SelectionAt(i);
		subject = adaptor->subject;
		if (subject->AllRead())
			continue;
		// has an unread article--open it
		Article* article = NULL;
		if (subject->Selected())
			article = subject->FirstUnreadArticle();
		else {
			// find the first selected article, and select the next unread article
			// at or after it
			int numArticles = subject->NumArticles();
			for (int j=0; j<numArticles; j++) {
				if (subject->ArticleAt(j)->Selected()) {
					// find next unread
					for (; j<numArticles; j++) {
						if (!subject->ArticleAt(j)->IsRead()) {
							article = subject->ArticleAt(j);
							break;
							}
						}
					break;
					}
				}
			}
		if (article) {
			OpenArticle(article, subject);
			openedSomething = true;
			}
		}
	// if all are read, select and read the next unread
	if (!openedSomething && numSelections > 0) {
		SubjectTreeAdaptor* lastSelected =
			(SubjectTreeAdaptor*) tree->LastSelectedObject();
		int newSelection = tree->IndexOf(lastSelected);
		while (true) {
			// try the next item
			newSelection += 1;
			adaptor = (SubjectTreeAdaptor*) tree->ObjectAt(newSelection);
			if (adaptor == NULL)
				break;
			subject = adaptor->subject;
			if (!subject->AllRead()) {
				// found one to read
				DeselectAll();
				Select(newSelection);
				OpenArticle(subject->FirstUnreadArticle(), subject);
				break;
				}
			}
		}
}


