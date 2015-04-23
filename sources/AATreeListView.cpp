/* AATreeListView.cpp */

#include "AATreeListView.h"
#include "ListableTree.h"
#include "ListableObject.h"
#include <Window.h>
#include <Message.h>
#include <Application.h>
#include <ScrollBar.h>
#include <ScrollView.h>

const float AATreeListView::SmallStepSize = 12;


AATreeListView::AATreeListView(BRect frame, ListableTree* treeIn, const char* name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags | B_WILL_DRAW | B_FRAME_EVENTS | B_NAVIGABLE),
	  tree(treeIn)
{
}


void AATreeListView::SetTree(ListableTree* newTree)
{
	tree = newTree;
	UpdateScrollBar();
	Invalidate();
}


ListableTree* AATreeListView::Tree()
{
	return tree;
}


void AATreeListView::InvalidateItem(ListableObject* item)
{
	BRect rect = Bounds();
	rect.top = tree->YForObject(item);
	rect.bottom = rect.top + item->DisplayHeight();
	Invalidate(rect);
}


void AATreeListView::SelectItem(ListableObject* item)
{
	tree->SelectObject(item);
	InvalidateItem(item);
}


void AATreeListView::DeselectItem(ListableObject* item)
{
	tree->DeselectObject(item);
	InvalidateItem(item);
}


void AATreeListView::SelectAll()
{
	tree->SelectAllObjects();
	Invalidate();
}


void AATreeListView::DeselectAll()
{
	// since we may not have a backing bitmap, we're gonna avoid all unnecessary
	// invalidation
	int numSelections = tree->NumSelections();
	if (numSelections == 0)
		return;
	ListableObject* singleItem = NULL;
	if (tree->NumSelections() == 1)
		singleItem = tree->SelectedObjectAt(0);

	tree->DeselectAllObjects();

	if (singleItem)
		InvalidateItem(singleItem);
	else
		Invalidate();
}


void AATreeListView::Select(int index)
{
	SelectItem((ListableObject*) tree->ObjectAt(index));
}


void AATreeListView::Deselect(int index)
{
	DeselectItem((ListableObject*) tree->ObjectAt(index));
}


void AATreeListView::SelectFirst()
{
	SelectItem((ListableObject*) tree->ObjectAt(0));
}


int AATreeListView::NumSelections()
{
	return tree->NumSelections();
}


ListableObject* AATreeListView::SelectionAt(int index)
{
	return tree->SelectedObjectAt(index);
}


void AATreeListView::OpenSelections()
{
	int numSelections = tree->NumSelections();
	for (int i=0; i<numSelections; i++)
		tree->SelectedObjectAt(i)->Open(this);
}


void AATreeListView::ScrollToSelection()
{
	// see if a selected item is already visible
	ListableObject* firstItem = tree->FirstSelectedObject();
	if (firstItem == NULL)
		return;
	ListableObject* lastItem = tree->LastSelectedObject();
	BRect bounds = Bounds();
	int firstY = tree->YForObject(firstItem);
	if (firstY >= bounds.top &&
			tree->YForObject(lastItem) + lastItem->DisplayHeight() - 1 <= bounds.bottom)
		return;

	// scroll
	ScrollTo(0, firstY);
}


int AATreeListView::IndexOf(ListableObject* object)
{
	return tree->IndexOf(object);
}


int AATreeListView::NumObjects()
{
	return tree->NumObjects();
}


void AATreeListView::Draw(BRect updateRect)
{
	// figure out which items we're drawing
	int firstItem = tree->IndexForY(updateRect.top);
	if (firstItem < 0)
		firstItem = 0;
	int32 lastItem = tree->IndexForY(updateRect.bottom);
	int32 numItems = tree->NumObjects();
	if (lastItem < 0 || lastItem >= numItems)
		lastItem = numItems - 1;

	// draw
	BRect itemRect = Bounds();
	itemRect.top = tree->YAtIndex(firstItem);
	for (int32 i=firstItem; i <= lastItem; i++) {
		// draw item
		ListableObject* item = dynamic_cast<ListableObject*>(tree->ObjectAt(i));
		if (item == NULL)
			continue;
		itemRect.bottom = itemRect.top + item->DisplayHeight() - 1;
		item->Draw(itemRect, this);

		// bump itemRect
		itemRect.top = itemRect.bottom + 1;
		}

	// clear any left-over area
	if (itemRect.top < updateRect.bottom) {
		rgb_color whiteColor = { 255, 255, 255, 0 };
		SetLowColor(whiteColor);
		itemRect.bottom = updateRect.bottom;
		FillRect(itemRect, B_SOLID_LOW);
		}
}


void AATreeListView::MouseDown(BPoint point)
{
	if (!IsFocus())
		MakeFocus();

	int oldHeight = tree->DisplayHeight();

	// tell the object
	int index = tree->IndexForY(point.y);
	if (index < 0)
		return;
	ListableObject* item = dynamic_cast<ListableObject*>(tree->ObjectAt(index));
	if (item == NULL)
		return;
	BRect itemRect = Bounds();
	itemRect.top = tree->YAtIndex(index);
	itemRect.bottom = itemRect.top + item->DisplayHeight() - 1;
	BMessage* message = Window()->CurrentMessage();
	int clicks = message->FindInt32("clicks");
	int modifiers = message->FindInt32("modifiers");
	item->MouseDown(point, itemRect, clicks, modifiers, this);
	// might have changed expansion or selection, so readjust the tree
	tree->ObjectChanged(item);
	if (tree->DisplayHeight() != oldHeight)
		UpdateScrollBar();
}


void AATreeListView::KeyDown(const char* bytes, int32 numBytes)
{
	uint32 modifiers;

	switch (bytes[0]) {
		case B_ENTER:
		case ' ':
			OpenSelections();
			break;

		case B_UP_ARROW:
			{
			ListableObject* firstSelected = tree->FirstSelectedObject();
			if (firstSelected) {
				modifiers = Window()->CurrentMessage()->FindInt32("modifiers");
				firstSelected->SelectUp(modifiers, this);
				}
			else if (tree->NumObjects() > 0) {
				Select(tree->NumObjects() - 1);
				ScrollToSelection();
				}
			}
			break;

		case B_DOWN_ARROW:
			{
			ListableObject* lastSelected = tree->LastSelectedObject();
			if (lastSelected) {
				modifiers = Window()->CurrentMessage()->FindInt32("modifiers");
				lastSelected->SelectDown(modifiers, this);
				}
			else if (tree->NumObjects() > 0) {
				Select(0);
				ScrollToSelection();
				}
			}
			break;

		case B_PAGE_UP:
			{
			float visibleHeight = Bounds().Height() + 1;
			ScrollBy(0, -(visibleHeight - SmallStepSize));
			}
			break;

		case B_PAGE_DOWN:
			{
			float visibleHeight = Bounds().Height() + 1;
			ScrollBy(0, visibleHeight - SmallStepSize);
			}
			break;

		case B_HOME:
			ScrollTo(0, 0);
			break;

		case B_END:
			{
			float visibleHeight = Bounds().Height();
			ScrollTo(0, tree->DisplayHeight() - visibleHeight);
			}
			break;

		case B_BACKSPACE:
		case B_DELETE:
/***
			if (selection >= 0)
				RemoveSelectedItem(selection);
***/
			break;

		default:
			BView::KeyDown(bytes, numBytes);
			break;
		}
}


void AATreeListView::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
	if (transit == B_ENTERED_VIEW || message == NULL)
		be_app->SetCursor(B_HAND_CURSOR);
	// don't bother calling inherited, since we know it does nothing
}


void AATreeListView::TargetedByScrollView(BScrollView* scrollerIn)
{
	scroller = scrollerIn;
}


void AATreeListView::AttachedToWindow()
{
	UpdateScrollBar();
}


void AATreeListView::FrameResized(float width, float height)
{
	UpdateScrollBar();
}


void AATreeListView::NumItemsChanged()
{
	UpdateScrollBar();
	Invalidate();
}


void AATreeListView::ItemDisplayChanged(ListableObject* item)
{
	tree->ObjectChanged(item);
	UpdateScrollBar();
	Invalidate();
}


void AATreeListView::UpdateScrollBar()
{
	// sanity clause
	if (scroller == NULL)
		return;

	BScrollBar* scrollBar = scroller->ScrollBar(B_VERTICAL);
	float visibleHeight = Bounds().Height() + 1;
	float dataHeight = tree->DisplayHeight();
	float scrollMax = dataHeight - visibleHeight;
	if (scrollMax < 0)
		scrollMax = 0;
	scrollBar->SetRange(0, scrollMax);
	scrollBar->SetSteps(SmallStepSize, visibleHeight - SmallStepSize);
}


