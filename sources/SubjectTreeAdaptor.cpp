/* SubjectTreeAdaptor.cpp */

#include "SubjectTreeAdaptor.h"
#include "Subject.h"
#include "Article.h"
#include "AATreeListView.h"
#include "SubjectsView.h"
#include "ListableTree.h"
#include "OpenArticleThread.h"
#include "NewsgroupWind.h"
#include "FontUtils.h"
#include "ListColors.h"
#include "Preferences.h"
#include "qstring.h"
#include "Messages.h"
#include <stdio.h>

float SubjectTreeAdaptor::itemHeight = 0;
BFont SubjectTreeAdaptor::font;
BFont SubjectTreeAdaptor::extraInfoFont;
float SubjectTreeAdaptor::articleItemIndent;
float SubjectTreeAdaptor::linesWidth;
bool SubjectTreeAdaptor::showLines;
bool SubjectTreeAdaptor::showDates;
const float SubjectTreeAdaptor::expanderLeft = 3;
const float SubjectTreeAdaptor::itemLeft = 10;
const float SubjectTreeAdaptor::numArticlesRight = 14;
const float SubjectTreeAdaptor::subjectLeft = 17;
const float SubjectTreeAdaptor::articleItemIndentEms = 2;
const float SubjectTreeAdaptor::articleMarkLeft = 4;
const float SubjectTreeAdaptor::articleLeft = 12;
const float SubjectTreeAdaptor::itemRight = 4;
const float SubjectTreeAdaptor::baseline = 3;


int SubjectTreeAdaptor::DisplayHeight()
{
	SetupFont();

	if (subject->Expanded())
		return (int) (itemHeight + subject->NumArticles() * itemHeight);
	else
		return (int) itemHeight;
}


bool SubjectTreeAdaptor::Selected()
{
	if (subject->Selected())
		return true;
	else {
		int numArticles = subject->NumArticles();
		for (int i=0; i<numArticles; i++) {
			if (subject->ArticleAt(i)->Selected())
				return true;
			}
		}
	return false;
}


void SubjectTreeAdaptor::SetSelected(bool newSelected)
{
	subject->SetSelected(newSelected);
	if (!newSelected)
		DeselectAllArticles();
}


void SubjectTreeAdaptor::Draw(BRect rect, AATreeListView* view)
{
	/***/
	float subjectHeight = itemHeight;
	float articleHeight = itemHeight;

	// clear the expander area
	BRect expanderRect = rect;
	expanderRect.right = rect.left + itemLeft - 1;
	view->SetLowColor(ListColors::bgndColor);
	view->FillRect(expanderRect, B_SOLID_LOW);

	SetupFont();
	view->SetFont(&font);

	// draw the expander
	if (subject->NumArticles() > 1) {
		BPoint expanderStart(rect.left + expanderLeft, rect.top + subjectHeight - baseline - 1);
		DrawExpander(view, expanderStart, subject->Expanded());
		}

	// draw the subject
	BRect itemRect = rect;
	itemRect.left += itemLeft;
	itemRect.bottom = itemRect.top + subjectHeight - 1;
	DrawSubject(itemRect, view);

	// draw the articles
	if (subject->Expanded()) {
		// clear the sliver to the left of the sub-items
		BRect clearRect = rect;
		clearRect.top += subjectHeight;
		clearRect.left += itemLeft;
		clearRect.right = clearRect.left + articleItemIndent - 1;
		view->SetLowColor(ListColors::bgndColor);
		view->FillRect(clearRect, B_SOLID_LOW);
		// draw the articles
		itemRect.left += articleItemIndent;
		itemRect.top = itemRect.bottom + 1;
		itemRect.bottom = itemRect.top + articleHeight - 1;
		int numArticles = subject->NumArticles();
		for (int i=0; i<numArticles; i++) {
			DrawArticle(i, itemRect, view);
			// bump itemRect
			itemRect.top = itemRect.bottom + 1;
			itemRect.bottom = itemRect.top + articleHeight - 1;
			}
		}
}


void SubjectTreeAdaptor::MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view)
{
	/***/
	float subjectHeight = itemHeight;
	float articleHeight = itemHeight;

	// expander
	BRect expanderRect = rect;
	expanderRect.left += expanderLeft;
	expanderRect.right = rect.left + itemLeft;
	if (expanderRect.Contains(where)) {
		BPoint expanderStart(rect.left + expanderLeft, rect.top + subjectHeight - baseline - 1);
		subject->SetExpanded(TrackExpander(view, expanderStart, subject->Expanded()));
		view->Invalidate();
		}

	// double-click opens
	else if (clicks > 1)
		view->OpenSelections();

	// command key reverses sense of selection
	else if ((modifiers & B_COMMAND_KEY) != 0) {
		if (where.y < rect.top + subjectHeight) {
			// toggle selection of subject
			subject->SetSelected(!subject->Selected());
			}
		else {
			// toggle selection of article
			int whichArticle = (int) ((where.y - rect.top - subjectHeight) / articleHeight);
			Article* article = subject->ArticleAt(whichArticle);
			article->SetSelected(!article->Selected());
			}
		view->InvalidateItem(this);
		}

	// select this, or an article
	else {
		view->DeselectAll();
		if (where.y < rect.top + subjectHeight) {
			// select subject
			view->SelectItem(this);
			}
		else {
			// select an article
			int whichArticle = (int) ((where.y - rect.top - subjectHeight) / articleHeight);
			subject->ArticleAt(whichArticle)->SetSelected(true);
			view->InvalidateItem(this);
			}
		((SubjectsView*) view)->SetSelectDirection(None);
		}
}


void SubjectTreeAdaptor::Open(AATreeListView* view)
{
	Article* article;

	Newsgroup* newsgroup = ((NewsgroupWind*) view->Window())->GetNewsgroup();

	// open selected articles if there are any
	bool haveSelectedArticle = false;
	int numArticles = subject->NumArticles();
	for (int i=0; i<numArticles; i++) {
		article = subject->ArticleAt(i);
		if (article->Selected()) {
			OpenArticle(article, newsgroup);
			haveSelectedArticle = true;
			}
		}

	// otherwise open the first unread article
	if (!haveSelectedArticle) {
		article = subject->FirstUnreadArticle();
		if (article == NULL)
			article = subject->ArticleAt(0);
		OpenArticle(article, newsgroup);
		}

	view->InvalidateItem(this);
}


void SubjectTreeAdaptor::SelectDown(uint32 modifiers, AATreeListView* viewIn)
{
	SubjectsView* view = (SubjectsView*) viewIn;
	if ((modifiers & B_SHIFT_KEY) != 0 && view->SelectDirection() == Up) {
		// deselect downwards
		// first get the first selected adaptor, since we're the last
		SubjectTreeAdaptor* firstSelectedAdaptor =
			(SubjectTreeAdaptor*) view->SelectionAt(0);
		Subject* firstSelectedSubject = firstSelectedAdaptor->subject;
		// find first selected article
		int numArticles = firstSelectedSubject->NumArticles();
		int firstSelected = 0;
		for (; firstSelected < numArticles; firstSelected++) {
			if (firstSelectedSubject->ArticleAt(firstSelected)->Selected())
				break;
			}
		// deselect
		if (firstSelected >= numArticles)
			firstSelectedSubject->SetSelected(false);
		else
			firstSelectedSubject->ArticleAt(firstSelected)->SetSelected(false);
		view->ItemDisplayChanged(firstSelectedAdaptor);
		// adjust selection direction
		if (view->NumSelections() == 1 &&
				((SubjectTreeAdaptor*) view->SelectionAt(0))->NumSelections() == 1)
			view->SetSelectDirection(None);
		}
	else if (!subject->Expanded() || (modifiers & B_OPTION_KEY) != 0) {
		ListableObject::SelectDown(modifiers, view);
		view->SetSelectDirection((modifiers & B_SHIFT_KEY) == 0 ? None : Down);
		}
	else {
		// find the last selected article
		int lastSelected = -1;
		int numArticles = subject->NumArticles();
		for (int i=numArticles-1; i>=0; --i) {
			if (subject->ArticleAt(i)->Selected()) {
				lastSelected = i;
				break;
				}
			}
		// select
		if (lastSelected >= numArticles - 1)
			ListableObject::SelectDown(modifiers, view);
		else {
			if ((modifiers & B_SHIFT_KEY) == 0)
				view->DeselectAll();
			subject->ArticleAt(lastSelected + 1)->SetSelected(true);
			view->ItemDisplayChanged(this);
			view->SetSelectDirection((modifiers & B_SHIFT_KEY) == 0 ? None : Down);
			}
		}
}


void SubjectTreeAdaptor::SelectUp(uint32 modifiers, AATreeListView* viewIn)
{
	SubjectsView* view = (SubjectsView*) viewIn;
	if ((modifiers & B_SHIFT_KEY) != 0 && view->SelectDirection() == Down) {
		// deselect upwards
		// first get the last selected adaptor, since we're the first
		SubjectTreeAdaptor* lastSelectedAdaptor =
			(SubjectTreeAdaptor*) view->SelectionAt(view->NumSelections() - 1);
		Subject* lastSelectedSubject = lastSelectedAdaptor->subject;
		// find last selected article
		int lastSelected = lastSelectedSubject->NumArticles() - 1;
		for (; lastSelected >= 0; --lastSelected) {
			if (lastSelectedSubject->ArticleAt(lastSelected)->Selected())
				break;
			}
		// deselect
		if (lastSelected < 0)
			lastSelectedSubject->SetSelected(false);
		else
			lastSelectedSubject->ArticleAt(lastSelected)->SetSelected(false);
		view->ItemDisplayChanged(lastSelectedAdaptor);
		// adjust selection direction
		if (view->NumSelections() == 1 &&
				((SubjectTreeAdaptor*) view->SelectionAt(0))->NumSelections() == 1)
			view->SetSelectDirection(None);
		}
	else if ((modifiers & B_OPTION_KEY) != 0) {
		// skips to (previous) subject line
		if (subject->Selected())
			ListableObject::SelectUp(modifiers, view);
		else {
			view->DeselectAll();
			view->SelectItem(this);
			}
		view->SetSelectDirection(None);
		}
	else if (!subject->Expanded() || subject->Selected()) {
		int index = view->IndexOf(this);
		if (index > 0) {
			if ((modifiers & B_SHIFT_KEY) == 0)
				view->DeselectAll();
			((SubjectTreeAdaptor*) view->Tree()->ObjectAt(index - 1))->SelectLast(view);
			}
		else
			ListableObject::SelectUp(modifiers, view);
		view->SetSelectDirection((modifiers & B_SHIFT_KEY) == 0 ? None : Up);
		}
	else {
		// find the first selected article
		int firstSelected = -1;
		int numArticles = subject->NumArticles();
		for (int i=0; i<numArticles; i++) {
			if (subject->ArticleAt(i)->Selected()) {
				firstSelected = i;
				break;
				}
			}
			// at this point "firstSelected" is guaranteed to have been found:
			// if there were no selected articles, the subject line is selected
			// (or SelectUp() wouldn't have been called), which would have been
			// handled above
		// select
		if ((modifiers & B_SHIFT_KEY) == 0)
			view->DeselectAll();
		if (firstSelected == 0) {
			// first item is selected; select the subject line
			subject->SetSelected(true);
			}
		else {
			// select previous article
			subject->ArticleAt(firstSelected - 1)->SetSelected(true);
			}
		view->ItemDisplayChanged(this);
		view->SetSelectDirection((modifiers & B_SHIFT_KEY) == 0 ? None : Up);
		}
}


void SubjectTreeAdaptor::SelectLast(AATreeListView* view)
{
	if (!subject->Expanded())
		view->SelectItem(this);
	else {
		// select the last article
		subject->ArticleAt(subject->NumArticles() - 1)->SetSelected(true);
		view->ItemDisplayChanged(this);
		}
	view->ScrollToSelection();
}


void SubjectTreeAdaptor::FontChanged()
{
	// uncache info
	itemHeight = 0;
}


void SubjectTreeAdaptor::ExtraInfoDisplayChanged()
{
	showLines = Prefs()->GetBoolPref("showLines");
	showDates = Prefs()->GetBoolPref("showDates");
}


int SubjectTreeAdaptor::NumSelections()
{
	int numSelections = 0;
	if (subject->Selected())
		numSelections += 1;
	int numArticles = subject->NumArticles();
	for (int i=0; i<numArticles; i++) {
		if (subject->ArticleAt(i)->Selected())
			numSelections += 1;
		}
	return numSelections;
}


bool SubjectTreeAdaptor::HasExtraInfo()
{
	return (showLines || showDates);
}


void SubjectTreeAdaptor::DrawSubject(BRect itemRect, BView* view)
{
	// setup
	SetupColors(view, itemRect, subject->Selected(), subject->FilterScore());

	// draw the mark
	int numArticles = subject->NumArticles();
	if (subject->AllRead()) {
		BPoint markStart(itemRect.left + numArticlesRight - view->StringWidth("√"),
		                 itemRect.bottom - baseline);
		view->DrawString("√", markStart);
		}
	else if (numArticles > 1) {
		int numUnread = subject->NumUnreadArticles();
		if (numUnread > 99)
			numUnread = 99;
		view->PushState();
		view->SetHighColor(ListColors::numArticlesColor);
		qstring numArticlesStr(numUnread);
		BPoint numArticlesStart(itemRect.left + numArticlesRight
		                        	- view->StringWidth(numArticlesStr.c_str()),
		                        itemRect.bottom - baseline);
		view->DrawString(numArticlesStr.c_str(), numArticlesStart);
		view->PopState();
		}

	// draw subject
	string_slice str = subject->GetSubject();
	view->DrawString(str.begin(), str.length(),
	                 BPoint(itemRect.left + subjectLeft, itemRect.bottom - baseline));
}


void SubjectTreeAdaptor::DrawArticle(int whichArticle, BRect itemRect, BView* view)
{
	// setup
	Article* article = subject->ArticleAt(whichArticle);
	SetupColors(view, itemRect, article->Selected(), article->FilterScore());
	float y = itemRect.bottom - baseline;

	// draw the mark
	if (article->IsRead()) {
		BPoint markStart(itemRect.left + articleMarkLeft, y);
		view->DrawString("√", markStart);
		}

	// draw author
	string_slice author = article->Author();
	view->DrawString(author.begin(), author.length(),
	                 BPoint(itemRect.left + articleLeft, y));

	// draw extra info
	if (showLines || showDates) {
		view->PushState();
		view->SetFont(&extraInfoFont);
		char str[64];
		float x = itemRect.right - itemRight;

		// lines
		if (showLines) {
			int count = article->Lines();
			char unit = 0;
			if (count >= 1000) {
				count /= 1000;
				unit = 'k';
				}
			sprintf(str, "%d%c", count, unit);
			view->DrawString(str, BPoint(x - extraInfoFont.StringWidth(str), y));
			x -= linesWidth;
			}

		// date
		if (showDates) {
			time_t date = article->Date();
			strftime(str, 64, "%Y.%m.%d", gmtime(&date));
			view->DrawString(str, BPoint(x - extraInfoFont.StringWidth(str), y));
			view->PopState();
			}
		}
}


void SubjectTreeAdaptor::SetupColors(BView* view, BRect itemRect, bool selected, float filterScore)
{
	// clear background
	if (selected)
		view->SetLowColor(ListColors::selectedBgndColor);
	else
		view->SetLowColor(filterScore > 0 ? ListColors::hilitedBgndColor :
		                  ListColors::bgndColor);
	view->FillRect(itemRect, B_SOLID_LOW);

	// set up to draw
	view->SetHighColor(ListColors::textColor);
}


void SubjectTreeAdaptor::DeselectAllArticles()
{
	int numArticles = subject->NumArticles();
	for (int i=0; i<numArticles; i++)
		subject->ArticleAt(i)->SetSelected(false);
}


void SubjectTreeAdaptor::OpenArticle(Article* article, Newsgroup* newsgroup)
{
	article->SetRead(true);
	OpenArticleThread* thread =
		new OpenArticleThread(newsgroup, article->ArticleNo(),
		                      subject->GetSubject(), article->Lines());
	thread->Go();
}


void SubjectTreeAdaptor::SetupFont()
{
	if (itemHeight != 0)
		return;

	// main info
	FontUtils::StringToFont(Prefs()->GetStringPref("listFont", defaultListFont), &font);
	font_height fontInfo;
	font.GetHeight(&fontInfo);
	itemHeight = ceil(fontInfo.ascent + fontInfo.descent + fontInfo.leading);
	articleItemIndent = articleItemIndentEms * font.StringWidth("m");

	// extra info
	FontUtils::StringToFont(Prefs()->GetStringPref("extraInfoFont", defaultExtraInfoFont),
	                        &extraInfoFont);
	extraInfoFont.GetHeight(&fontInfo);
	float extraInfoHeight = ceil(fontInfo.ascent + fontInfo.descent + fontInfo.leading);
	if (extraInfoHeight > itemHeight)
		itemHeight = extraInfoHeight;
	linesWidth = extraInfoFont.StringWidth(" 000");
}


