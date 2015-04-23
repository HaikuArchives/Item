/* TrackGroupDragThread.cpp */

#include "TrackGroupDragThread.h"
#include "ScrolledListView.h"
#include "Newsrc.h"
#include "Newsgroup.h"
#include "DragUtils.h"
#include <Message.h>
#include <Window.h>
#include <List.h>



TrackGroupDragThread::TrackGroupDragThread(ScrolledListView* viewIn, Newsrc* newsrcIn, const BMessage* messageIn)
	: view(viewIn), newsrc(newsrcIn), message(new BMessage(*messageIn))
{
}


TrackGroupDragThread::~TrackGroupDragThread()
{
	delete message;
}


void TrackGroupDragThread::Action()
{
	// track the drag
	int insertionIndex = -1;
	while (true) {
		BWindow* wind = view->Window();
		if (wind == NULL || !wind->Lock())
			return;

		// get the mouse, and stop dragging if it leaves the view horizontally
		// (not vertically, since we want that to autoscroll)
		BPoint point;
		uint32 buttons;
		view->GetMouse(&point, &buttons, true);
		if (buttons == 0) {
			view->Invalidate();
			wind->Unlock();
			break;
			}
		BRect bounds = view->Bounds();
		if (point.x < bounds.left || point.x > bounds.right) {
			view->Invalidate();
			wind->Unlock();
			return;
			}

		insertionIndex = view->TrackInsertionStep(point, insertionIndex);

		wind->Unlock();

		snooze(50000);
		}

	InsertDragAt(insertionIndex);
}


void TrackGroupDragThread::InsertDragAt(int index)
{
	int i;

	BList* groupNames = DragUtils::GetDraggedGroups(message);

	// add the groups
	int numDraggedGroups = groupNames->CountItems();
	for (i=0; i<numDraggedGroups; i++) {
		string_slice groupName = *(string_slice*) groupNames->ItemAt(i);
		if (newsrc->NewsgroupNamed(groupName) == NULL) {
			newsrc->AddNewsgroupAt(new Newsgroup(groupName, newsrc), index);
			index++;
			}
		}
	if (view->Window()->Lock()) {
		view->NumItemsChanged();
		view->Window()->Unlock();
		}

	// delete groupNames
	for (i=0; i<numDraggedGroups; i++)
		delete (string_slice*) groupNames->ItemAt(i);
	delete groupNames;
}


