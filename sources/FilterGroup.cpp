/* FilterGroup.cpp */

#include "FilterGroup.h"
#include "qstring.h"
#include "Filter.h"
#include "FiltersView.h"
#include <List.h>

const float FilterGroup::groupHeight = 14;
const float FilterGroup::expanderLeft = 3;
const float FilterGroup::itemLeft = 14;
const float FilterGroup::nameLeft = 3;
const float FilterGroup::filterHeight = 12;
const float FilterGroup::filterIndent = 8;
const float FilterGroup::baseline = 3;
const rgb_color FilterGroup::bgndColor = { 255, 255, 255, 255 };
const rgb_color FilterGroup::hiliteBgndColor = { 192, 192, 192, 255 };


FilterGroup::FilterGroup(string_slice groupNameIn)
	: expanded(true)
{
	groupName = new qstring(groupNameIn);
	filters = new BList();
}


FilterGroup::~FilterGroup()
{
	delete groupName;
	int numFilters = filters->CountItems();
	for (int i=0; i<numFilters; i++)
		delete (Filter*) filters->ItemAt(i);
	delete filters;
}


void FilterGroup::ApplyFilters(Article* article, string_slice subject)
{
	int numFilters = NumFilters();
	for (int i=0; i<numFilters; i++)
		FilterAt(i)->ApplyTo(article, subject);
}


void FilterGroup::AddFilter(Filter* filter)
{
	filters->AddItem(filter);
}


void FilterGroup::EditFilterString(Filter* filter, BRect rect, FiltersView* view)
{
	int whichFilter = filters->IndexOf(filter);
	if (whichFilter < 0)
		return;
	BRect itemRect = rect;
	itemRect.left += itemLeft + filterIndent;
	itemRect.top = rect.top + groupHeight + whichFilter * filterHeight;
	itemRect.bottom = itemRect.top + filterHeight - 1;
	filter->EditString(itemRect, view);
}


string_slice FilterGroup::GroupName() const
{
	return *groupName;
}


int FilterGroup::NumFilters()
{
	return filters->CountItems();
}


Filter* FilterGroup::FilterAt(int index)
{
	return (Filter*) filters->ItemAt(index);
}


bool FilterGroup::isLessThan(const OrderedObject& otherObject)
{
	return string_slice(*groupName) < ((const FilterGroup*) &otherObject)->GroupName();
}


bool FilterGroup::isEqualTo(const OrderedObject& otherObject)
{
	return string_slice(*groupName) == ((const FilterGroup*) &otherObject)->GroupName();
}


int FilterGroup::DisplayHeight()
{
	float height = groupHeight;
	if (expanded)
		height += filterHeight * filters->CountItems();
	return (int) height;
}


void FilterGroup::Draw(BRect rect, AATreeListView* view)
{
	// draw the expander
	DrawExpander(view, BPoint(rect.left + expanderLeft, rect.top + groupHeight - baseline - 1),
	             expanded);

	// draw the group header
	BRect itemRect = rect;
	itemRect.left += itemLeft;
	itemRect.bottom = itemRect.top + groupHeight - 1;
	view->PushState();
	view->SetLowColor(selected ? hiliteBgndColor : bgndColor);
	view->FillRect(itemRect, B_SOLID_LOW);
	view->DrawString(groupName->c_str(),
	                 BPoint(itemRect.left + nameLeft, itemRect.bottom - baseline));
	view->PopState();

	// draw the filters
	if (expanded) {
		itemRect.left += filterIndent;
		itemRect.top = itemRect.bottom + 1;
		itemRect.bottom = itemRect.top + filterHeight - 1;
		int numFilters = filters->CountItems();
		for (int i=0; i<numFilters; i++) {
			Filter* filter = (Filter*) filters->ItemAt(i);
			filter->Draw(itemRect, (FiltersView*) view);
			// bump itemRect
			itemRect.top += filterHeight;
			itemRect.bottom += filterHeight;
			}
		}
}


void FilterGroup::MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view)
{
	((FiltersView*) view)->FinishStringEdit();

	// expander
	float groupBottom = rect.top + groupHeight - 1;
	if (where.x < rect.left + itemLeft && where.y <= groupBottom) {
		BPoint expanderStart(rect.left + expanderLeft, groupBottom - baseline);
		expanded = TrackExpander(view, expanderStart, expanded);
		view->Invalidate();
		return;
		}

	// select this
	view->DeselectAll();
	view->SelectItem(this);

	// pass it on to the filter if appropriate
	if (where.x >= rect.left + itemLeft && where.y > groupBottom) {
		int whichFilter = (int) ((where.y - groupBottom) / filterHeight);
		Filter* filter = FilterAt(whichFilter);
		if (filter) {
			BRect itemRect = rect;
			itemRect.left += itemLeft + filterIndent;
			itemRect.top = groupBottom + 1 + whichFilter * filterHeight;
			itemRect.bottom = itemRect.top + filterHeight - 1;
			filter->MouseDown(where, itemRect, clicks, modifiers, (FiltersView*) view);
			view->InvalidateItem(this);
			}
		}
}


