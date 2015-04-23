/* FiltersView.cpp */

#include "FiltersView.h"
#include "Filter.h"
#include "Filters.h"
#include "FilterGroup.h"
#include "AddGroupDlog.h"
#include "qstring.h"
#include "Messages.h"
#include <TextControl.h>
#include <Message.h>



FiltersView::FiltersView(BRect frame, ListableTree* tree, const char* name, uint32 resizingMode, uint32 flags)
	: AATreeListView(frame, tree, name, resizingMode, flags),
	  stringEditor(NULL), editingFilter(NULL)
{
}


FiltersView::~FiltersView()
{
	FinishStringEdit();
}


void FiltersView::EditFilterString(Filter* filter, BRect rect)
{
	// finish up an existing one
	if (stringEditor)
		FinishStringEdit();

	// start the new editor
	editingFilter = filter;
	stringEditor = new BTextControl(rect, "Filter String", NULL,
	                                qstring(filter->GetString()).c_str(),
	                                new BMessage(EditDoneMessage),
	                                B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	stringEditor->SetTarget(this);
	AddChild(stringEditor);
	stringEditor->MakeFocus();
}


void FiltersView::FinishStringEdit()
{
	if (stringEditor == NULL)
		return;
	editingFilter->SetString(string_slice(stringEditor->Text()));
	RemoveChild(stringEditor);
	delete stringEditor;
	stringEditor = NULL;
	editingFilter = NULL;
}


void FiltersView::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case EditDoneMessage:
			FinishStringEdit();
			break;

		case AddGroupMessage:
			{
			string_slice groupName(message->FindString("groupName"));
			Filters* filters = (Filters*) tree;
			if (filters->GetFilterGroup(groupName) == NULL) {
				filters->AddFilterGroup(new FilterGroup(groupName));
				NumItemsChanged();
				}
			}
			break;

		case NewGroupMessage:
			new AddGroupDlog("New Filter Group", BMessenger(this));
			break;

		case NewFilterMessage:
			{
			FilterGroup* filterGroup = (FilterGroup*) SelectionAt(0);
			if (filterGroup) {
				Filter* newFilter = new Filter();
				filterGroup->AddFilter(newFilter);
				ItemDisplayChanged(filterGroup);

				// start edit of filter string
				BRect itemRect = Bounds();
				itemRect.top = tree->YForObject(filterGroup);
				itemRect.bottom = itemRect.top + filterGroup->DisplayHeight() - 1;
				filterGroup->Draw(itemRect, this);
					// not only do we want it drawn, but Filter::EditString()
					// doesn't work correctly until the Filter has been drawn
				filterGroup->EditFilterString(newFilter, itemRect, this);
				}
			}
			break;

		default:
			AATreeListView::MessageReceived(message);
			break;
		}
}


