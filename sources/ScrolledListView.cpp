/* ScrolledListView.cpp */

#include "ScrolledListView.h"
#include <Window.h>
#include <Application.h>
#include <ScrollBar.h>
#include <ScrollView.h>

const rgb_color ScrolledListView::insertionIndicatorColor = { 128, 128, 128, 255 };


ScrolledListView::ScrolledListView(BRect frame, const char* name, uint32 resizingMode, uint32 flags)
	: BView(frame, name, resizingMode, flags), selection(-1), scroller(NULL),
	  active(true)
{
}


int ScrolledListView::NumItems()
{
	/** pure virtual **/
	return 0;
}


float ScrolledListView::ItemHeight()
{
	/** pure virtual **/
	// real height *in pixels*
	return 0;
}


void ScrolledListView::DrawItem(int32 itemIndex, BRect itemRect, bool selected)
{
	/** pure virtual **/
}


void ScrolledListView::OpenSelectedItem(int32 selectedIndex)
{
}


void ScrolledListView::RemoveSelectedItem(int32 selectedIndex)
{
}


void ScrolledListView::SelectionChanging(int32 newSelection, int32 oldSelection)
{
}


void ScrolledListView::ActivationChanging(bool toActive)
{
}


bool ScrolledListView::CanRearrange()
{
	return false;
}


void ScrolledListView::ItemMoved(int32 oldIndex, int32 newIndex)
{
}


void ScrolledListView::Select(int32 index)
{
	int32 newSelection = index;
	if (newSelection < 0 || newSelection >= NumItems())
		newSelection = -1;
	if (selection != newSelection) {
		SelectionChanging(newSelection, selection);
		selection = newSelection;
		Invalidate();
		}
}


void ScrolledListView::ScrollToSelection()
{
	if (selection < 0)
		return;

	// see if the selected item is already visible
	float itemHeight = ItemHeight();
	BRect bounds = Bounds();
	int32 firstItem = (int) (bounds.top / itemHeight);
	int32 lastItem = (int) (bounds.bottom / itemHeight);
	if (selection >= firstItem && selection <= lastItem)
		return;

	// scroll
	ScrollTo(0, selection * itemHeight);
}


void ScrolledListView::InvalidateItem(int index)
{
	float itemHeight = ItemHeight();
	BRect itemRect = Bounds();
	itemRect.top = index * itemHeight;
	itemRect.bottom = itemRect.top + itemHeight - 1;
	Invalidate(itemRect);
}


void ScrolledListView::Activate()
{
	if (active)
		return;

	SetFlags(Flags() | B_NAVIGABLE);
	active = true;
	ActivationChanging(true);
	Invalidate();
}


void ScrolledListView::Deactivate()
{
	if (!active)
		return;

	SetFlags(Flags() & ~B_NAVIGABLE);
	active = false;
	ActivationChanging(false);
	Invalidate();
}


void ScrolledListView::Draw(BRect updateRect)
{
	// figure out which items we're drawing
	float itemHeight = ItemHeight();
	int32 firstItem = (int) (updateRect.top / itemHeight);
	if (firstItem < 0)
		firstItem = 0;
	int32 lastItem = (int) ((updateRect.bottom + itemHeight - 1) / itemHeight);
	int32 numItems = NumItems();
	if (lastItem >= numItems)
		lastItem = numItems - 1;

	// draw
	BRect itemRect = Bounds();
	itemRect.top = firstItem * itemHeight;
	itemRect.bottom = itemRect.top + itemHeight - 1;
	for (int32 i=firstItem; i <= lastItem; i++) {
		// draw item
		DrawItem(i, itemRect, i == selection);

		// bump itemRect
		itemRect.top = itemRect.bottom + 1;
		itemRect.bottom = itemRect.top + itemHeight - 1;
		}

	// clear any left-over area
	if (itemRect.top < updateRect.bottom) {
		itemRect.bottom = updateRect.bottom;
		FillRect(itemRect, B_SOLID_LOW);
		}
}


void ScrolledListView::MouseDown(BPoint point)
{
	int32 clicks; 
	Window()->CurrentMessage()->FindInt32("clicks", &clicks);

	Activate();
	if (!IsFocus())
		MakeFocus();

	int32 newSelection = (int) (point.y / ItemHeight());
	if (newSelection < 0 || newSelection >= NumItems())
		newSelection = -1;

	// double-click opens
	if (selection == newSelection && selection >= 0 && clicks > 1)
		OpenSelectedItem(selection);

	// first click selects, maybe moves
	else {
		Select(newSelection);
		if (CanRearrange())
			TrackRearrangement(point);
		}
}


void ScrolledListView::KeyDown(const char* bytes, int32 numBytes)
{
	switch (bytes[0]) {
		case B_ENTER:
		case ' ':
			if (selection >= 0)
				OpenSelectedItem(selection);
			break;

		case B_UP_ARROW:
			int32 newSelection;
			if (selection < 0)
				newSelection = NumItems() - 1;
			else if (selection > 0)
				newSelection = selection - 1;
			else
				newSelection = selection;
			Select(newSelection);
			ScrollToSelection();
			break;

		case B_DOWN_ARROW:
			if (selection < NumItems() - 1)
				Select(selection + 1);
			ScrollToSelection();
			break;

		case B_PAGE_UP:
			{
			float itemHeight = ItemHeight();
			float visibleHeight = Bounds().Height() + 1;
			ScrollBy(0, -(visibleHeight - itemHeight));
			}
			break;

		case B_PAGE_DOWN:
			{
			float itemHeight = ItemHeight();
			float visibleHeight = Bounds().Height() + 1;
			ScrollBy(0, visibleHeight - itemHeight);
			}
			break;

		case B_HOME:
			ScrollTo(0, 0);
			break;

		case B_END:
			{
			float visibleHeight = Bounds().Height();
			ScrollTo(0, NumItems() * ItemHeight() - visibleHeight);
			}
			break;

		case B_BACKSPACE:
		case B_DELETE:
			if (selection >= 0)
				RemoveSelectedItem(selection);
			break;

		default:
			BView::KeyDown(bytes, numBytes);
			break;
		}
}


void ScrolledListView::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
	if (transit == B_ENTERED_VIEW || message == NULL)
		be_app->SetCursor(B_HAND_CURSOR);
	// don't bother calling inherited, since we know it does nothing
}


void ScrolledListView::TargetedByScrollView(BScrollView* scrollerIn)
{
	scroller = scrollerIn;
}


void ScrolledListView::AttachedToWindow()
{
	UpdateScrollBar();
}


void ScrolledListView::FrameResized(float width, float height)
{
	UpdateScrollBar();
}


void ScrolledListView::NumItemsChanged()
{
	UpdateScrollBar();
	if (selection >= NumItems())
		selection = -1;
	Invalidate();
}


void ScrolledListView::UpdateScrollBar()
{
	// sanity clause
	if (scroller == NULL)
		return;

	BScrollBar* scrollBar = scroller->ScrollBar(B_VERTICAL);
	float itemHeight = ItemHeight();
	float visibleHeight = Bounds().Height() + 1;
	float dataHeight = NumItems() * itemHeight;
	float scrollMax = dataHeight - visibleHeight;
	if (scrollMax < 0)
		scrollMax = 0;
	scrollBar->SetRange(0, scrollMax);
	scrollBar->SetSteps(itemHeight, visibleHeight - itemHeight);
}


void ScrolledListView::TrackRearrangement(BPoint point)
{
/***
	static const rgb_color indicatorColor = { 128, 128, 128, 256 };
***/

	// sanity clause
	if (selection < 0)
		return;

	// first, because the selection might have changed, redraw the whole thing
	BRect bounds = Bounds();
	Draw(bounds);

	// track the movement
	int newIndex = -1;
	while (true) {
		uint32 buttons;
		GetMouse(&point, &buttons, true);
			// UNDOCUMENTED FACT ABOUT GetMouse():
			// If the "checkQueue" argument is true, GetMouse() will cause
			// the invalid region to be redrawn!  In our case we don't want
			// that, because sometimes it means the indicator gets clobbered,
			// since scrolling invalidates areas.
			// BUT, if "checkQueue" is false, something goes wrong after
			// you've held the mouse down for a while (about six seconds on
			// my machine), and the tracking loop starts to respond really
			// slowly.  Is the queue filling up?  Maybe with MOUSE_MOVED
			// messages, but I tried cleaning them out and it didn't help.
			// It didn't even delay the onset of the problem.
			// SO:  We leave "checkQueue" as true, just like the example in
			// the BeBook, but we do this GetMouse() call *before* we do
			// our drawing.  This means a bit of flashiness since items can
			// be drawn twice, but guarantees that everything will look right.
		if (buttons == 0)
			break;

		newIndex = TrackInsertionStep(point, newIndex);
		}

/***
	// track the movement
	float itemHeight = ItemHeight();
	int32 numItems = NumItems();
	int32 newIndex = -1;
	uint32 buttons = 1;
	while (buttons) {
		// autoscroll
		if (point.y < bounds.top) {
			if (bounds.top > 0) {
				ScrollBy(0, -itemHeight);
				bounds.OffsetBy(0, -itemHeight);
				}
			point.y = bounds.top;
			}
		else if (point.y > bounds.bottom) {
			if (bounds.bottom < numItems * itemHeight - 1) {
				ScrollBy(0, itemHeight);
				bounds.OffsetBy(0, itemHeight);
				}
			point.y = bounds.bottom + 1;	// need the +1 to let it get beyond the last item
			}

		// figure out where it is now
		int32 curNewIndex = point.y / itemHeight;
		if (curNewIndex < 0)
			curNewIndex = 0;
		else if (curNewIndex > numItems)	// can move beyond the last item
			curNewIndex = numItems;

		GetMouse(&point, &buttons, true);
			// UNDOCUMENTED FACT ABOUT GetMouse():
			// If the "checkQueue" argument is true, GetMouse() will cause
			// the invalid region to be redrawn!  In our case we don't want
			// that, because sometimes it means the indicator gets clobbered,
			// since scrolling invalidates areas.
			// BUT, if "checkQueue" is false, something goes wrong after
			// you've held the mouse down for a while (about six seconds on
			// my machine), and the tracking loop starts to respond really
			// slowly.  Is the queue filling up?  Maybe with MOUSE_MOVED
			// messages, but I tried cleaning them out and it didn't help.
			// It didn't even delay the onset of the problem.
			// SO:  We leave "checkQueue" as true, just like the example in
			// the BeBook, but we do this GetMouse() call *before* we do
			// our drawing.  This means a bit of flashiness since items can
			// be drawn twice, but guarantees that everything will look right.
			// We can get away with updating "point" and "buttons" here (instead
			// of at the end of the loop as you'd normally do) because they're
			// not used from here on.

		// draw
		if (curNewIndex != newIndex) {
			// redraw items bordering old indicator, to clear it
			if (newIndex >= 0) {
				if (newIndex > 0)
					DrawItemAt(newIndex - 1);
				if (newIndex < numItems)
					DrawItemAt(newIndex);
				else {
					// need to clean up bottom
					BRect bottomRect = bounds;
					bottomRect.top = newIndex * itemHeight;
					FillRect(bottomRect, B_SOLID_LOW);
					}
				}

			// draw new indicator
			newIndex = curNewIndex;
			SetHighColor(indicatorColor);
			SetPenSize(2.0);
			float indicatorY = newIndex * itemHeight;
			StrokeLine(BPoint(bounds.left, indicatorY),
			           BPoint(bounds.right, indicatorY));
			SetPenSize(1.0);
			}

		// go around again
		Flush();
		snooze(50000);	// BeBook-recommended snooze time is 20000, but we don't need to be that greedy
		}
***/

	// move the item
	if (newIndex >= 0) {
		if (newIndex > selection)
			newIndex -= 1;
		if (newIndex != selection) {
			ItemMoved(selection, newIndex);
			selection = newIndex;
			}
		}

	Invalidate();
}


int ScrolledListView::TrackInsertionStep(BPoint point, int prevInsertionIndex)
{
	BRect bounds = Bounds();
	float itemHeight = ItemHeight();
	int32 numItems = NumItems();

	// autoscroll
	if (point.y < bounds.top) {
		if (bounds.top > 0) {
			ScrollBy(0, -itemHeight);
			bounds.OffsetBy(0, -itemHeight);
			}
		point.y = bounds.top;
		}
	else if (point.y > bounds.bottom) {
		if (bounds.bottom < numItems * itemHeight - 1) {
			ScrollBy(0, itemHeight);
			bounds.OffsetBy(0, itemHeight);
			}
		point.y = bounds.bottom + 1;	// need the +1 to let it get beyond the last item
		}

	// figure out where it is now
	int32 curInsertionIndex = (int) (point.y / itemHeight);
	if (curInsertionIndex < 0)
		curInsertionIndex = 0;
	else if (curInsertionIndex > numItems)	// can move beyond the last item
		curInsertionIndex = numItems;

	// draw
	if (curInsertionIndex != prevInsertionIndex) {
		// redraw items bordering old indicator, to clear it
		if (prevInsertionIndex >= 0) {
			if (prevInsertionIndex > 0)
				DrawItemAt(prevInsertionIndex - 1);
			if (prevInsertionIndex < numItems)
				DrawItemAt(prevInsertionIndex);
			else {
				// need to clean up bottom
				BRect bottomRect = bounds;
				bottomRect.top = prevInsertionIndex * itemHeight;
				FillRect(bottomRect, B_SOLID_LOW);
				}
			}

		// draw new indicator
		SetHighColor(insertionIndicatorColor);
		SetPenSize(2.0);
		float indicatorY = curInsertionIndex * itemHeight;
		StrokeLine(BPoint(bounds.left, indicatorY),
		           BPoint(bounds.right, indicatorY));
		SetPenSize(1.0);
		}

	Flush();

	return curInsertionIndex;
}


void ScrolledListView::DrawItemAt(int32 index)
{
	float itemHeight = ItemHeight();
	BRect itemRect = Bounds();
	itemRect.top = index * itemHeight;
	itemRect.bottom = itemRect.top + itemHeight - 1;
	DrawItem(index, itemRect, (index == selection));
}


