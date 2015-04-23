/* GroupName.cpp */

#include "GroupName.h"
#include "GroupsListView.h"
#include "ListColors.h"

float GroupName::baseline = -1;
const int GroupName::displayHeight = 12;
const float GroupName::nameLeft = 4;


GroupName::GroupName(string_slice nameIn)
	: name(nameIn)
{
}


bool GroupName::isLessThan(const OrderedObject& otherObject)
{
	return name < ((const GroupName*) &otherObject)->name;
}


bool GroupName::isEqualTo(const OrderedObject& otherObject)
{
	return name == ((const GroupName*) &otherObject)->name;
}


int GroupName::DisplayHeight()
{
	return displayHeight;
}


void GroupName::Draw(BRect rect, AATreeListView* view)
{
	// set up cached baseline
	if (baseline < 0) {
		font_height fontInfo;
		be_plain_font->GetHeight(&fontInfo);
		baseline = ceil(fontInfo.descent);
		}

	// set up colors
	view->SetHighColor(ListColors::textColor);
	view->SetLowColor(selected ? ListColors::selectedBgndColor : ListColors::bgndColor);
	view->FillRect(rect, B_SOLID_LOW);

	// draw the name
	view->DrawString(name.begin(), name.length(),
	                 BPoint(rect.left + nameLeft, rect.bottom - baseline));
}


void GroupName::MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view)
{
	// if already selected, initiate drag
	if (Selected())
		((GroupsListView*) view)->StartDrag(rect);

	// otherwise, select
	else {
		if ((modifiers & B_COMMAND_KEY) == 0)
			view->DeselectAll();
		view->SelectItem(this);
		}
}


string_slice GroupName::Name()
{
	return name;
}


