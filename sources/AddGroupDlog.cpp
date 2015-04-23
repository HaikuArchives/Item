/* AddGroupDlog.cpp */

#include "AddGroupDlog.h"
#include "Messages.h"
#include <View.h>
#include <TextControl.h>
#include <Button.h>
#include <Screen.h>

const rgb_color AddGroupDlog::bgndColor = { 220, 220, 220, 255 };
const float AddGroupDlog::margin = 10;
const float AddGroupDlog::groupViewDivider = 60;
const float AddGroupDlog::buttonWidth = 80;
const float AddGroupDlog::buttonHeight = 20;
const float AddGroupDlog::buttonHSpacing = 16;
const float AddGroupDlog::windHeight = 80;
const float AddGroupDlog::defaultWindWidth = 300;
const int32 AddGroupDlog::AcceptedMessage = 'Acpt';


AddGroupDlog::AddGroupDlog(const char* title, const BMessenger& targetIn)
	: BWindow(BRect(0, 0, defaultWindWidth, windHeight), title, B_TITLED_WINDOW,
	          B_NOT_V_RESIZABLE | B_NOT_ZOOMABLE),
	  target(targetIn)
{
	// set up background color
	BView* bgndView = new BView(Frame(), "", B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED);
	bgndView->SetViewColor(bgndColor);
	AddChild(bgndView);

	// group view
	BRect groupViewFrame(margin, margin, defaultWindWidth - margin, margin);
	groupView = new BTextControl(groupViewFrame, "", "Newsgroup:", NULL, NULL,
	                             B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	groupView->SetDivider(groupViewDivider);
	bgndView->AddChild(groupView);

	// add button
	const float addButtonRight = defaultWindWidth - margin;
	const float buttonTop = windHeight - margin - buttonHeight;
	BRect buttonFrame(addButtonRight - buttonWidth, buttonTop,
	                  addButtonRight, buttonTop + buttonHeight);
	addButton = new BButton(buttonFrame, "", "Add",
	                        new BMessage(AcceptedMessage),
	                        B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	bgndView->AddChild(addButton);
	SetDefaultButton(addButton);

	// cancel button
	buttonFrame.OffsetBy(-(buttonWidth + buttonHSpacing), 0);
	BButton* cancelButton = new BButton(buttonFrame, "", "Cancel",
	                                    new BMessage(B_QUIT_REQUESTED),
	                                    B_FOLLOW_RIGHT | B_FOLLOW_BOTTOM);
	bgndView->AddChild(cancelButton);

	// center on the screen
	BRect screenRect = BScreen(this).Frame();
	MoveTo((screenRect.left + screenRect.right - defaultWindWidth) / 2,
	       (screenRect.top + screenRect.bottom - windHeight) / 2);

	groupView->MakeFocus();
	Show();
}


void AddGroupDlog::DispatchMessage(BMessage* message, BHandler* handler)
{
	switch (message->what) {
		case B_KEY_DOWN:
			// intercept ESC key to close the window
			int8 key;
			message->FindInt8("byte", &key);
			if (key == B_ESCAPE)
				PostMessage(B_QUIT_REQUESTED);
			else if (key == ' ' && CurrentFocus() == groupView->TextView()) {
				// disallow spaces in the groupView
				}
			else
				BWindow::DispatchMessage(message, handler);
			break;

		default:
			BWindow::DispatchMessage(message, handler);
			break;
		}

	// update the addButton's enabledness
	addButton->SetEnabled(groupView->Text()[0] != 0);
}


void AddGroupDlog::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case AcceptedMessage:
			{
			BMessage addGroupMessage(AddGroupMessage);
			addGroupMessage.AddString("groupName", groupView->Text());
			target.SendMessage(&addGroupMessage);
			Quit();
			}
			break;

		default:
			BWindow::MessageReceived(message);
			break;
		}
}


