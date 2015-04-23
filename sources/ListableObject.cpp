/* ListableObject.cpp */

#include "ListableObject.h"
#include "AATreeListView.h"

const float ListableObject::expanderHeight = 7;
const float ListableObject::expanderWidth = 4;
const rgb_color ListableObject::expanderColor = { 128, 128, 128, 0 };
const rgb_color ListableObject::expanderTrimColor = { 128, 128, 255, 0 };


ListableObject::ListableObject()
	: selected(false)
{
}


int ListableObject::DisplayHeight()
{
	/** pure virtual **/
	return 0;
}


void ListableObject::Draw(BRect rect, AATreeListView* view)
{
	/** pure virtual **/
}


void ListableObject::MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view)
{
	/** pure virtual **/
}


void ListableObject::Open(AATreeListView* view)
{
	/** pure virtual **/
}


bool ListableObject::Selected()
{
	return selected;
}


void ListableObject::SetSelected(bool newSelected)
{
	selected = newSelected;
}


void ListableObject::SelectDown(uint32 modifiers, AATreeListView* view)
{
	int index = view->IndexOf(this);
	if ((modifiers & B_SHIFT_KEY) == 0)
		view->DeselectAll();
	view->Select(index < view->NumObjects() - 1 ? index + 1 : index);
	view->ScrollToSelection();
}


void ListableObject::SelectUp(uint32 modifiers, AATreeListView* view)
{
	int index = view->IndexOf(this);
	if ((modifiers & B_SHIFT_KEY) == 0)
		view->DeselectAll();
	view->Select(index > 0 ? index - 1 : 0);
	view->ScrollToSelection();
}


void ListableObject::DrawExpander(BView* view, BPoint bottomLeft, bool expanded, bool tracking)
{
	view->PushState();

	// figure out the points of the triangle
	BPoint triangle[3];
	if (!expanded) {
		triangle[0] = bottomLeft;
		triangle[1].x = bottomLeft.x;
		triangle[1].y = bottomLeft.y - expanderHeight;
		triangle[2].x = bottomLeft.x + expanderWidth;
		triangle[2].y = bottomLeft.y - expanderHeight / 2;
		}
	else {
//***		float top = bottomLeft.y - expanderHeight + expanderWidth / 2;
		float top = bottomLeft.y - expanderWidth;
		triangle[0].x = bottomLeft.x;
		triangle[0].y = top;
		triangle[1].x = bottomLeft.x + expanderHeight;
		triangle[1].y = top;
		triangle[2].x = bottomLeft.x + expanderHeight / 2;
		triangle[2].y = top + expanderWidth;
		}

	// draw
	if (!tracking) {
		view->SetHighColor(expanderColor);
		view->FillTriangle(triangle[0], triangle[1], triangle[2]);
		view->SetHighColor(expanderTrimColor);
		view->StrokeTriangle(triangle[0], triangle[1], triangle[2]);
		}
	else {
		view->SetHighColor(expanderTrimColor);
		view->FillTriangle(triangle[0], triangle[1], triangle[2]);
		}

	view->PopState();
}


bool ListableObject::TrackExpander(BView* view, BPoint bottomLeft, bool expanded)
{
	// start draw
	DrawExpander(view, bottomLeft, expanded, true);
	view->Sync();

	BRect expanderRect(bottomLeft.x, bottomLeft.y - expanderHeight,
	                   bottomLeft.x + expanderHeight, bottomLeft.y);
	BPoint mousePoint, lastMouse(-1, -1);
	uint32 buttons;
	bool curFlipped = true;
	while (true) {
		view->GetMouse(&mousePoint, &buttons);
		if (buttons == 0)
			break;
		if (mousePoint == lastMouse) {
			snooze(20000);
			continue;
			}
		bool flipped = expanderRect.Contains(mousePoint);
		if (flipped != curFlipped) {
			DrawExpander(view, bottomLeft, expanded, flipped);
			view->Sync();
			}
		lastMouse = mousePoint;
		curFlipped = flipped;
		}

	// don't bother to clean up draw, as the whole item'll have to be redrawn anyway

	return curFlipped ^ expanded;
}


