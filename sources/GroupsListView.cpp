/* GroupsListView.cpp */

#include "GroupsListView.h"
#include "GroupsList.h"
#include "GroupName.h"
#include "qstring.h"
#include <Message.h>



GroupsListView::GroupsListView(BRect frame, GroupsList* groupsListIn)
	: AATreeListView(frame, groupsListIn->GetTree()), groupsList(groupsListIn)
{
	groupsList->AttachedToView(this);
}


GroupsListView::~GroupsListView()
{
	groupsList->DetachedFromView();
}


void GroupsListView::Draw(BRect updateRect)
{
	if (groupsList->Lock()) {
		AATreeListView::Draw(updateRect);
		groupsList->Unlock();
		}
}


void GroupsListView::StartDrag(BRect itemRect)
{
	// build the text
	qstring text;
	int numSelections = NumSelections();
	for (int i=0; i<numSelections; i++) {
		GroupName* group = (GroupName*) SelectionAt(i);
		text += group->Name();
		text += "\n";
		}

	// build the message
	BMessage message(B_MIME_DATA);
	message.AddData("text/plain", B_MIME_TYPE, text.data(), text.length());

	// drag
	DragMessage(&message, itemRect);
}


