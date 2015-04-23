/* Filter.cpp */

#include "Filter.h"
#include "qstring.h"
#include "Article.h"
#include "FiltersView.h"
#include "TextReader.h"
#include <File.h>
#include <PopUpMenu.h>
#include <MenuItem.h>

const float Filter::textLeft = 3;
const float Filter::baseline = 3;
const rgb_color Filter::bgndColor = { 255, 255, 255, 0 };
const float Filter::strEditXOffset = -4;
const float Filter::strEditYOffset = -5;
const float Filter::strEditRightMargin = 6;
const char* Filter::actionStrings[] = { "Hilite", "Kill", NULL };
const char* Filter::headerStrings[] = { "Subject", "Author", NULL };


Filter::Filter()
	: action(Hilite), header(BySubject)
{
	str = new qstring();
}


Filter::Filter(FilterAction actionIn, FilterHeader headerIn, string_slice stringIn)
	: action(actionIn), header(headerIn), str(new qstring(stringIn))
{
}


Filter::~Filter()
{
	delete str;
}


void Filter::ApplyTo(Article* article, string_slice subject)
{
	// see if this filter applies to the articles
	bool applies = false;
	switch (header) {
		case BySubject:
			applies = subject.containsCI(*str);
			break;

		case ByAuthor:
			applies = article->Author().containsCI(*str);
			break;
		}
	if (!applies)
		return;

	// do the appropriate action
	switch (action) {
		case Hilite:
			article->SetFilterScore(article->FilterScore() + 1);
			break;

		case Kill:
			article->SetRead(true);
			break;
		}
}


void Filter::WriteToFile(BFile* file)
{
	string_slice tab("\t");
	string_slice eol("\n");

	file->Write(tab.begin(), tab.length());
	string_slice actionStr = actionStrings[action];
	file->Write(actionStr.begin(), actionStr.length());
	file->Write(tab.begin(), tab.length());
	string_slice headerStr = headerStrings[header];
	file->Write(headerStr.begin(), headerStr.length());
	file->Write(tab.begin(), tab.length());
	file->Write(str->data(), str->length());
	file->Write(eol.begin(), eol.length());
}


void Filter::ReadFrom(TextReader* reader)
{
	// already past the empty field

	action = (FilterAction) IndexFor(reader->NextTabField(), actionStrings);
	header = (FilterHeader) IndexFor(reader->NextTabField(), headerStrings);
	*str = reader->NextTabField();
}


string_slice Filter::GetString()
{
	return *str;
}


void Filter::SetString(string_slice newStr)
{
	*str = newStr;
}


void Filter::Draw(BRect rect, FiltersView* view)
{
	view->PushState();
	view->SetLowColor(bgndColor);
	view->FillRect(rect, B_SOLID_LOW);
	view->MovePenTo(rect.left + textLeft, rect.bottom - baseline);

	// action
	view->DrawString(actionStrings[action]);
	actionRight = view->PenLocation().x;
	view->DrawString(" if ");

	// header
	headerLeft = view->PenLocation().x;
	view->DrawString(headerStrings[header]);
	headerRight = view->PenLocation().x;
	view->DrawString(" contains ");

	// str
	qstring displayStr = "\"";
	displayStr += str;
	displayStr += '"';
	strLeft = view->PenLocation().x;
	view->DrawString(displayStr.c_str());

	view->PopState();
}


void Filter::MouseDown(BPoint where, BRect rect, int clicks, int modifiers, FiltersView* view)
{
	BPopUpMenu* menu;
	BMenuItem* selectedItem;
	BPoint popPoint;

	BRect clickToOpenRect = view->ConvertToScreen(rect);

	// action menu
	if (where.x < actionRight) {
		menu = new BPopUpMenu("Filter Action");
		BMenuItem* hiliteItem = new BMenuItem(actionStrings[Hilite], NULL);
		hiliteItem->SetMarked(action == Hilite);
		menu->AddItem(hiliteItem);
		BMenuItem* killItem = new BMenuItem(actionStrings[Kill], NULL);
		killItem->SetMarked(action == Kill);
		menu->AddItem(killItem);
		popPoint = view->ConvertToScreen(BPoint(rect.left + textLeft, rect.top));
		selectedItem = menu->Go(popPoint, false, true, clickToOpenRect);
		if (selectedItem == hiliteItem)
			action = Hilite;
		else if (selectedItem == killItem)
			action = Kill;
		delete menu;
		}

	// headerMenu
	else if (where.x >= headerLeft && where.x < headerRight) {
		menu = new BPopUpMenu("Filter Header");
		BMenuItem* subjectItem = new BMenuItem(headerStrings[BySubject], NULL);
		subjectItem->SetMarked(header == BySubject);
		menu->AddItem(subjectItem);
		BMenuItem* authorItem = new BMenuItem(headerStrings[ByAuthor], NULL);
		authorItem->SetMarked(header == ByAuthor);
		menu->AddItem(authorItem);
		popPoint = view->ConvertToScreen(BPoint(headerLeft, rect.top));
		selectedItem = menu->Go(popPoint, false, true, clickToOpenRect);
		if (selectedItem == subjectItem)
			header = BySubject;
		else if (selectedItem == authorItem)
			header = ByAuthor;
		delete menu;
		}

	else if (where.x >= strLeft) {
		EditString(rect, view);
		}

	/***/
}


void Filter::EditString(BRect rect, FiltersView* view)
{
	BRect editRect(strLeft, rect.top, rect.right - strEditRightMargin, rect.bottom);
	editRect.OffsetBy(strEditXOffset, strEditYOffset);
	view->EditFilterString(this, editRect);
}


int Filter::IndexFor(string_slice str, const char** strings)
{
	for (int index=0; ; index++) {
		const char* curString = strings[index];
		if (curString == NULL)
			break;
		if (str == curString)
			return index;
		}
	return -1;
}


