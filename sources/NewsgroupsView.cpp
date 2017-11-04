/* NewsgroupsView.cpp */

#include "NewsgroupsView.h"
#include "Newsrc.h"
#include "Newsgroup.h"
#include "OpenGroupThread.h"
#include "TrackGroupDragThread.h"
#include "NewsrcWind.h"
#include "NewsgroupWind.h"
#include "DragUtils.h"
#include "FontUtils.h"
#include "ListColors.h"
#include "Preferences.h"
#include "string_slice.h"
#include "qstring.h"
#include <Alert.h>

const float NewsgroupsView::leftMargin = 4;
const float NewsgroupsView::separationEms = 0.2;
const rgb_color NewsgroupsView::numArticlesColor = { 128, 128, 128, 255 };
const rgb_color NewsgroupsView::XtextColor = { 0, 0, 0, 255 };
const rgb_color NewsgroupsView::XbgndColor = { 255, 255, 255, 255 };
const rgb_color NewsgroupsView::XhiliteBgndColor = { 192, 192, 192, 255 };
const float NewsgroupsView::XnumArticlesRight = 27;
const float NewsgroupsView::XgroupNameLeft = 30;
const float NewsgroupsView::Xbaseline = 2;


NewsgroupsView::NewsgroupsView(BRect frame, Newsrc* newsrcIn)	// assumes ownership
	: ScrolledListView(frame, "Newsrc", B_FOLLOW_ALL_SIDES,
	                   B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE),
      newsrc(newsrcIn), itemHeight(0)
{
	SetViewColor(B_TRANSPARENT_32_BIT);
	newsrc->AttachedToView(this);
}


NewsgroupsView::~NewsgroupsView()
{
	delete newsrc;
}


int NewsgroupsView::NumItems()
{
	return newsrc->NumNewsgroups();
}


float NewsgroupsView::ItemHeight()
{
	SetupFont();
	return itemHeight;
}


void NewsgroupsView::DrawItem(int32 itemIndex, BRect itemRect, bool selected)
{
	SetupFont();

	// set up
	SetHighColor(ListColors::textColor);
	SetLowColor(selected ? ListColors::selectedBgndColor : ListColors::bgndColor);
	FillRect(itemRect, B_SOLID_LOW);
	SetFont(&font);

	// draw
	Newsgroup* newsgroup = newsrc->NewsgroupAt(itemIndex);
	// name
	string_slice name = newsgroup->Name();
	DrawString(name.begin(), name.length(),
	           BPoint(itemRect.left + groupNameLeft, itemRect.bottom - baseline));
	// numArticles
	int numArticles = newsgroup->NumUnreadArticles();
	if (numArticles > 0) {
		SetHighColor(numArticlesColor);
		qstring numArticlesStr(numArticles);
		if (numArticles > 9999) {
			numArticlesStr = qstring((numArticles + 500)/1000);
			numArticlesStr += "k";
			}
		float width = StringWidth(numArticlesStr.c_str());
		DrawString(numArticlesStr.c_str(),
		           BPoint(itemRect.left + leftMargin + numArticlesWidth - width,
		                  itemRect.bottom - baseline));
		}

	SetLowColor(ListColors::bgndColor);
}


void NewsgroupsView::OpenSelectedItem(int32 selectedIndex)
{
	Newsgroup* newsgroup = newsrc->NewsgroupAt(selectedIndex);
	BWindow* wind = newsgroup->Window();
	if (wind)
		wind->Activate();
	else
		((NewsrcWind*) Window())->StartThread(new OpenGroupThread(newsgroup));
}


void NewsgroupsView::RemoveSelectedItem(int32 selectedIndex)
{
	// confirm
	const char* message =
		"Are you sure you want to delete the selected group?  You won't be able to undo it.";
	BAlert* alert =
		new BAlert("Confirm group deletion", message, "Delete", "Cancel");
	alert->SetShortcut(1, B_ESCAPE);
	alert->SetShortcut(0, 'd');
	int buttonIndex = alert->Go();
	if (buttonIndex != 0)
		return;

	// remove
	newsrc->RemoveNewsgroupAt(selectedIndex);
	NumItemsChanged();
}


bool NewsgroupsView::CanRearrange()
{
	return true;
}


void NewsgroupsView::ItemMoved(int32 oldIndex, int32 newIndex)
{
	Newsgroup* newsgroup = newsrc->RemoveNewsgroupAt(oldIndex);
	newsrc->AddNewsgroupAt(newsgroup, newIndex);
}


void NewsgroupsView::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
	if (transit == B_ENTERED_VIEW && message != NULL && DragUtils::HasNewsgroups(message))
		(new TrackGroupDragThread(this, newsrc, message))->Go();
	else
		ScrolledListView::MouseMoved(point, transit, message);
}


void NewsgroupsView::PrefChanged(string_slice prefName)
{
	if (prefName == "listBgndColor" || prefName == "listSelectedBgndColor" ||
			prefName == "listTextColor") {
		Window()->Lock();
		Invalidate();
		Window()->Unlock();
		}
	else if (prefName == "listFont") {
		itemHeight = 0;	// uncache
		SetupFont();
		Window()->Lock();
		NumItemsChanged();
		Window()->Unlock();
		}
}


void NewsgroupsView::SetupFont()
{
	if (itemHeight != 0)
		return;

	FontUtils::StringToFont(Prefs()->GetStringPref("listFont", defaultListFont), &font);
	font_height fontInfo;
	font.GetHeight(&fontInfo);
	itemHeight = ceil(fontInfo.ascent + fontInfo.descent + fontInfo.leading);
	baseline = ceil(fontInfo.descent);
	float emWidth = font.StringWidth("m");

	numArticlesWidth = font.StringWidth("9999");
	groupNameLeft = leftMargin + numArticlesWidth + separationEms * emWidth;
}


