/* BaseListItem.cpp */

#include "BaseListItem.h"

const rgb_color BaseListItem::HiliteColor = { 192, 192, 192, 255 };
const rgb_color BaseListItem::TextColor = { 0, 0, 0, 255 };
const float BaseListItem::Baseline = 3;


BaseListItem::BaseListItem(bool isSubItem)
	: BListItem((isSubItem ? 1 : 0), false)
{
}


bool BaseListItem::Open(Newsgroup* newsgroup)
{
	// returns true if selection should move to next item
	return false;
}


void BaseListItem::Mark(Newsgroup* newsgroup)
{
	/** pure virtual **/
}


void BaseListItem::Unmark(Newsgroup* newsgroup)
{
	/** pure virtual **/
}


void BaseListItem::SetupColors(BView* view, BRect itemRect)
{
	// clear background
	view->SetLowColor(IsSelected() ? HiliteColor : view->ViewColor());
	view->FillRect(itemRect, B_SOLID_LOW);

	// set up to draw
	view->SetHighColor(TextColor);
}


