/* ServerDlog.cpp */

#include "ServerDlog.h"
#include "Prefs.h"
#include <TextControl.h>
#include <Button.h>
#include <Message.h>
#include <StringView.h>
#include <Screen.h>
#include <Beep.h>

const float ServerDlog::WindWidth = 300;
const float ServerDlog::WindHeight = 120;
const float ServerDlog::ButtonWidth = 80;
const float ServerDlog::ButtonHeight = 20;
const float ServerDlog::ButtonSpacing = 20;
const float ServerDlog::Margin = 12;
const float ServerDlog::LabelHeight = 14;
const float ServerDlog::ServerControlInset = 20;
const rgb_color ServerDlog::BgndColor = { 220, 220, 220, 255 };


ServerDlog::ServerDlog()
	: BWindow(BRect(0, 0, WindWidth, WindHeight), "Server Dlog", B_MODAL_WINDOW, B_NOT_RESIZABLE),
	  doneSem(-1), wasDone(false)
{
	// set up for synchronous operation
	doneSem = create_sem(0, "ServerDlog synchronous");

	// set up background color
	BView* bgndView = new BView(Frame(), "", B_FOLLOW_ALL, B_WILL_DRAW | B_PULSE_NEEDED);
	bgndView->SetViewColor(BgndColor);
	AddChild(bgndView);

	// make the views
	// label
	BRect labelFrame(Margin, Margin, WindWidth - 2*Margin, Margin + LabelHeight);
	bgndView->AddChild(new BStringView(labelFrame, "", "Enter the address of your news server:"));
	// serverControl
	BRect serverControlFrame(0, 0, WindWidth, 100);
	serverControlFrame.InsetBy(Margin + ServerControlInset, 0);
	serverControl =
		new BTextControl(serverControlFrame, "", "Server address:", "", NULL,
		                 B_FOLLOW_LEFT | B_FOLLOW_TOP,
		                 B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED);
	bgndView->AddChild(serverControl);
	float width, height;
	serverControl->GetPreferredSize(&width, &height);
	float vSpacing =
		floor((WindHeight - 2*Margin - LabelHeight - ButtonHeight - height) / 2);
	serverControl->MoveTo(serverControlFrame.left, Margin + LabelHeight + vSpacing);
	serverControl->MakeFocus();
	// ok button
	BRect okButtonFrame(WindWidth - Margin - ButtonWidth,
	                    WindHeight - Margin - ButtonHeight,
	                    WindWidth - Margin,
	                    WindHeight - Margin);
	BButton* okButton =
		new BButton(okButtonFrame, "", "OK", new BMessage(B_QUIT_REQUESTED));
	bgndView->AddChild(okButton);
	okButton->MakeDefault(true);
	// cancel button
	BRect cancelButtonFrame = okButtonFrame;
	cancelButtonFrame.OffsetBy(-(ButtonWidth + ButtonSpacing), 0);
	bgndView->AddChild(new BButton(cancelButtonFrame, "", "Cancel", new BMessage(B_CANCEL)));

	// center on the screen
	BRect screenRect = BScreen(this).Frame();
	MoveTo((screenRect.left + screenRect.right - WindWidth) / 2,
	       (screenRect.top + screenRect.bottom - WindHeight) / 2);
}


ServerDlog::~ServerDlog()
{
	delete_sem(doneSem);
}


void ServerDlog::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_CANCEL:
			Close();
			break;

		case B_KEY_DOWN:
			int8 key;
			message->FindInt8("byte", 0, &key);
			if (key == B_ESCAPE)
				Close();
			break;

		default:
			BWindow::MessageReceived(message);
			break;
		}
}


void ServerDlog::DispatchMessage(BMessage* message, BHandler* handler)
{
	bool passMessage = true;
	switch (message->what) {
		case B_KEY_DOWN:
			// intercept ESC key
			int8 key;
			message->FindInt8("byte", 0, &key);
			if (key == B_ESCAPE) {
				Close();
				passMessage = false;
				}
			break;
		}

	if (passMessage)
		BWindow::DispatchMessage(message, handler);
}


bool ServerDlog::QuitRequested()
{
	// verify
	const char* server = serverControl->Text();
	if (server[0] == 0) {
		beep();
		return false;
		}

	// set the pref
	Prefs()->SetStringPref("nntpServer", server);
	wasDone = true;
	return true;
}


bool ServerDlog::Go()
{
	Show();
	acquire_sem(doneSem);
	bool retVal = wasDone;
	Close();
	return retVal;
}


