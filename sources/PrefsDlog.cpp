/* PrefsDlog.cpp */

#include "PrefsDlog.h"
#include "ColorPrefControl.h"
#include "FontPrefControl.h"
#include "NNTPApp.h"
#include "ConnectionPool.h"
#include "Preferences.h"
#include "ListColors.h"
#include "Messages.h"
#include "qstring.h"
#include <Button.h>
#include <TabView.h>
#include <TextControl.h>
#include <CheckBox.h>
#include <Box.h>
#include <Message.h>
#include <Screen.h>
#include <Beep.h>

// tab indexes
enum {
	serverTabIndex
};

const float PrefsDlog::windWidth = 440;
const float PrefsDlog::windHeight = 300;
const float PrefsDlog::tabInset = 5;
const float PrefsDlog::tabMargin = 8;
const float PrefsDlog::boxChildTop = 14;
const float PrefsDlog::itemVSpacing = 10;
const float PrefsDlog::buttonWidth = 80;
const float PrefsDlog::buttonHeight = 20;
const float PrefsDlog::buttonSpacing = 20;
const float PrefsDlog::buttonMargin = 12;
const float PrefsDlog::numberFieldWidth = 40;
const rgb_color PrefsDlog::bgndColor = { 216, 216, 216, 255 };


PrefsDlog::PrefsDlog()
	: BWindow(BRect(0, 0, windWidth, windHeight), "Preferences", B_TITLED_WINDOW,
	          B_NOT_RESIZABLE | B_NOT_ZOOMABLE)
{
	// create the bgndView
	BView* bgndView = new BView(Bounds(), "Background", B_FOLLOW_ALL, B_WILL_DRAW);
	bgndView->SetViewColor(bgndColor);
	AddChild(bgndView);

	// create the "OK" button
	BRect okButtonFrame(windWidth - buttonMargin - buttonWidth,
	                    windHeight - buttonMargin - buttonHeight,
	                    windWidth - buttonMargin,
	                    windHeight - buttonMargin);
	BButton* okButton =
		new BButton(okButtonFrame, "", "OK", new BMessage(B_QUIT_REQUESTED));
	bgndView->AddChild(okButton);
	okButton->MakeDefault(true);

	// cancel button
	BRect cancelButtonFrame = okButtonFrame;
	cancelButtonFrame.OffsetBy(-(buttonWidth + buttonSpacing), 0);
	bgndView->AddChild(new BButton(cancelButtonFrame, "", "Cancel", new BMessage(B_CANCEL)));

	// create the tabView
	BRect tabViewRect = Bounds();
	tabViewRect.InsetBy(tabInset, tabInset);
	tabViewRect.bottom = cancelButtonFrame.top - buttonMargin;
	tabView = new BTabView(tabViewRect, "tabView");
	tabView->SetViewColor(bgndColor);
	bgndView->AddChild(tabView);

	// set up some handy rects 'n' stuff
	BRect panelRect = tabView->Bounds();
	panelRect.InsetBy(tabInset, tabInset);
	BRect itemRect = panelRect;
	itemRect.OffsetTo(0, 0);
	itemRect.InsetBy(tabMargin, tabMargin);
	Preferences* prefs = Prefs();

	// create the "Server" panel
	BView* serverView = new BView(panelRect, "Server", B_FOLLOW_ALL, B_WILL_DRAW);
	serverView->SetViewColor(bgndColor);
	// serverControl
	serverControl =
		new BTextControl(itemRect, "", "Server address:", "", NULL,
		                 B_FOLLOW_LEFT | B_FOLLOW_TOP,
		                 B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED);
	serverControl->SetText(prefs->GetStringPref("nntpServer"));
	serverControl->ResizeToPreferred();
	serverView->AddChild(serverControl);
	// maxConnectionsControl
	maxConnectionsControl =
		new BTextControl(itemRect, "", "Maximum connections:", "", NULL,
		                 B_FOLLOW_LEFT | B_FOLLOW_TOP,
		                 B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED);
	int maxConnections = prefs->GetInt32Pref("maxConnections", ConnectionPool::defaultMaxConnections);
	maxConnectionsControl->SetText(qstring(maxConnections).c_str());
	maxConnectionsControl->ResizeToPreferred();
	maxConnectionsControl->ResizeTo(maxConnectionsControl->Divider() + numberFieldWidth,
	                                maxConnectionsControl->Frame().Height());
	serverView->AddChild(maxConnectionsControl);
	// userNameControl
	userNameControl =
		new BTextControl(itemRect, "", "User name:", "", NULL,
		                 B_FOLLOW_LEFT | B_FOLLOW_TOP,
		                 B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED);
	userNameControl->SetText(prefs->GetStringPref("userName"));
	userNameControl->ResizeToPreferred();
	serverView->AddChild(userNameControl);
	// passwordControl
	passwordControl =
		new BTextControl(itemRect, "", "Password:", "", NULL,
		                 B_FOLLOW_LEFT | B_FOLLOW_TOP,
		                 B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED);
	passwordControl->SetText(prefs->GetStringPref("password"));
	passwordControl->ResizeToPreferred();
	serverView->AddChild(passwordControl);
	// set up
	LayoutItemsIn(serverView);
	tabView->AddTab(serverView);

	// "Appearance" panel
	BView* appearanceView = new BView(panelRect, "Appearance", B_FOLLOW_ALL, B_WILL_DRAW);
	appearanceView->SetViewColor(bgndColor);
	// list colors
	BBox* listColorsBox = new BBox(itemRect);
	listColorsBox->SetLabel("Colors");
	appearanceView->AddChild(listColorsBox);
		// selected
	listSelectedColorControl =
		new ColorPrefControl(itemRect, "listSelectedBgndColor", "Selected:", ListColors::selectedBgndColor);
	listSelectedColorControl->SetViewColor(bgndColor);
	listSelectedColorControl->ResizeToPreferred();
	listColorsBox->AddChild(listSelectedColorControl);
		// hilited
	listHilitedColorControl =
		new ColorPrefControl(itemRect, "listHilitedBgndColor", "Hilited:", ListColors::hilitedBgndColor);
	listHilitedColorControl->SetViewColor(bgndColor);
	listHilitedColorControl->ResizeToPreferred();
	listColorsBox->AddChild(listHilitedColorControl);
		// bgnd
	listBgndColorControl =
		new ColorPrefControl(itemRect, "listBgndColor", "Background:", ListColors::bgndColor);
	listBgndColorControl->SetViewColor(bgndColor);
	listBgndColorControl->ResizeToPreferred();
	listColorsBox->AddChild(listBgndColorControl);
		// selected
	listTextColorControl =
		new ColorPrefControl(itemRect, "listTextColor", "Text:", ListColors::textColor);
	listTextColorControl->SetViewColor(bgndColor);
	listTextColorControl->ResizeToPreferred();
	listColorsBox->AddChild(listTextColorControl);
	// set up the listColorsBox
	DistributeItemsAcross(listColorsBox, boxChildTop);
	ResizeBox(listColorsBox);
	// listFontControl
	listFontControl = new FontPrefControl("listFont", "Font:", itemRect,
	                                      B_FOLLOW_LEFT | B_FOLLOW_TOP);
	listFontControl->SetViewColor(bgndColor);
	listFontControl->ResizeToPreferred();
	appearanceView->AddChild(listFontControl);
	// extraFontControl
	extraInfoFontControl =
		new FontPrefControl("extraInfoFont", "Extra info font:", itemRect,
		                    B_FOLLOW_LEFT | B_FOLLOW_TOP);
	extraInfoFontControl->SetViewColor(bgndColor);
	extraInfoFontControl->ResizeToPreferred();
	appearanceView->AddChild(extraInfoFontControl);
	// expandSubjectsControl
	expandSubjectsControl =
		MakeCheckbox("expandSubjectsByDefault", "Expand subjects by default", itemRect);
	appearanceView->AddChild(expandSubjectsControl);
	// showDatesControl
	showDatesControl = MakeCheckbox("showDates", "Show dates", itemRect);
	appearanceView->AddChild(showDatesControl);
	// showLinesControl
	showLinesControl = MakeCheckbox("showLines", "Show lines", itemRect);
	appearanceView->AddChild(showLinesControl);
	// set up
	LayoutItemsIn(appearanceView);
	tabView->AddTab(appearanceView);

	// create the "Binaries" panel
	BView* binariesView = new BView(panelRect, "Binaries", B_FOLLOW_ALL, B_WILL_DRAW);
	binariesView->SetViewColor(bgndColor);
	// binariesPathControl
	binariesPathControl =
		new BTextControl(itemRect, "", "Save binaries in:", "", NULL,
		                 B_FOLLOW_LEFT | B_FOLLOW_TOP,
		                 B_WILL_DRAW | B_NAVIGABLE | B_PULSE_NEEDED);
	binariesPathControl->SetText(prefs->GetStringPref("binariesPath"));
	ResizeToPreferredHeight(binariesPathControl);
	binariesView->AddChild(binariesPathControl);
	// set up
	LayoutItemsIn(binariesView);
	tabView->AddTab(binariesView);

	// position the window
	BRect screenRect = BScreen(this).Frame();
	BRect savedFrame = Prefs()->GetRectPref("prefsDlogFrame");
	if (!savedFrame.IsValid() || !screenRect.Contains(savedFrame)) {
		// either not saved or off the screen; center on the screen
		MoveTo((screenRect.left + screenRect.right - windWidth) / 2,
		       (screenRect.top + screenRect.bottom - windHeight) / 2);
		}
	else {
		// move to saved position
		MoveTo(savedFrame.LeftTop());
		}

	tabView->MakeFocus();
	tabView->Select(0);
	Show();
}


PrefsDlog::~PrefsDlog()
{
	((NNTPApp*) be_app)->WindowClosing(this);
}


void PrefsDlog::FrameMoved(BPoint screenPoint)
{
	Prefs()->SetRectPref("prefsDlogFrame", Frame());
}


void PrefsDlog::DispatchMessage(BMessage* message, BHandler* handler)
{
	bool passMessage = true;
	switch (message->what) {
		case B_KEY_DOWN:
			int8 key;
			message->FindInt8("byte", &key);
			BView* focusView = CurrentFocus();
			// intercept ESC key to close the window
			if (key == B_ESCAPE) {
				Close();
				passMessage = false;
				}
			// always allow return, tab, backspace, delete
			else if (key == B_ENTER || key == '\t' || key == B_BACKSPACE || key == B_DELETE)
				passMessage = true;
			// always allow command keys
			else if ((message->FindInt32("modifiers") & B_COMMAND_KEY) != 0)
				passMessage = true;
			// some items don't allow spaces
			else if (focusView == serverControl->TextView()) {
				passMessage = (key != ' ');
				}
			// some items only take positive numbers
			else if (focusView == maxConnectionsControl->TextView()) {
				passMessage = (key >= '0' && key <= '9');
				}
			break;
		}

	if (passMessage)
		BWindow::DispatchMessage(message, handler);
}


void PrefsDlog::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_CANCEL:
			Close();
			break;

		case PrefsCheckboxClicked:
			CheckboxClicked(message);
			break;

		default:
			BWindow::MessageReceived(message);
			break;
		}
}


bool PrefsDlog::QuitRequested()
{
	// verify
	bool ok = true;
	// server tab
	if (serverControl->Text()[0] == 0) {
		tabView->Select(serverTabIndex);
		serverControl->MakeFocus();
		ok = false;
		}
	if (string_slice(maxConnectionsControl->Text()).asInt() < 1) {
		tabView->Select(serverTabIndex);
		maxConnectionsControl->MakeFocus();
		ok = false;
		}
	// kick out if not ok
	if (!ok) {
		beep();
		return false;
		}

	// save
	Save();

	return true;
}


BCheckBox* PrefsDlog::MakeCheckbox(const char* prefName, const char* label, BRect itemRect)
{
	BMessage* message = new BMessage(PrefsCheckboxClicked);
	BCheckBox* checkbox = new BCheckBox(itemRect, "", label, message);
	int32 checkboxValue =
		(Prefs()->GetBoolPref(prefName) ? B_CONTROL_ON : B_CONTROL_OFF);
	checkbox->SetValue(checkboxValue);
	checkbox->ResizeToPreferred();
	return checkbox;
}


void PrefsDlog::LayoutItemsIn(BView* parentView)
{
	float y = tabMargin;
	int numChildren = parentView->CountChildren();
	for (int i=0; i<numChildren; i++) {
		BView* childView = parentView->ChildAt(i);
		childView->MoveTo(tabMargin, y);
		y += childView->Bounds().Height() + itemVSpacing;
		}
}


void PrefsDlog::DistributeItemsAcross(BView* parentView, float top)
{
	int i;
	BView* childView;

	// first find the total width
	float totalWidth = 0;
	int numChildren = parentView->CountChildren();
	for (i=0; i<numChildren; i++) {
		childView = parentView->ChildAt(i);
		totalWidth += childView->Frame().Width() + 1;
		}

	// distribute
	float parentWidth = (parentView->Frame().Width() + 1) - 2 * tabMargin;
	float spacing = (parentWidth - totalWidth) / (numChildren - 1);
	float x = tabMargin;
	for (i=0; i<numChildren; i++) {
		childView = parentView->ChildAt(i);
		childView->MoveTo(x, top);
		x += childView->Bounds().Width() + 1 + spacing;
		}
}


void PrefsDlog::ResizeBox(BView* view)
{
	int numChildren = view->CountChildren();
	float newHeight =
		(numChildren > 0 ?
		 view->ChildAt(numChildren - 1)->Frame().bottom + tabMargin :
		 view->Frame().bottom);
	view->ResizeTo(view->Frame().Width(), newHeight);
}


void PrefsDlog::ResizeToPreferredHeight(BTextControl* control)
{
	float origWidth = control->Bounds().Width();
	control->ResizeToPreferred();
	float correctDivider = control->Divider();
	control->ResizeTo(origWidth, control->Bounds().Height());
	control->SetDivider(correctDivider);
	// setting the divider doesn't resize the textView, so we have to do that ourself
	BTextView* textView = control->TextView();
	textView->ResizeTo(origWidth - correctDivider, textView->Bounds().Height());
}


void PrefsDlog::Save()
{
	Preferences* prefs = Prefs();

	// server prefs
	prefs->SetStringPref("nntpServer", serverControl->Text());
	prefs->SetInt32Pref("maxConnections",
	                    string_slice(maxConnectionsControl->Text()).asInt());
	prefs->SetStringPref("userName", userNameControl->Text());
	prefs->SetStringPref("password", passwordControl->Text());

	// color prefs
	listSelectedColorControl->Save();
	listHilitedColorControl->Save();
	listBgndColorControl->Save();
	listTextColorControl->Save();
	// other appearance prefs
	listFontControl->Save();
	extraInfoFontControl->Save();
	SaveCheckbox(expandSubjectsControl, "expandSubjectsByDefault");
	SaveCheckbox(showDatesControl, "showDates");
	SaveCheckbox(showLinesControl, "showLines");

	// binaries prefs
	prefs->SetStringPref("binariesPath", binariesPathControl->Text());

	prefs->Save();
}


void PrefsDlog::SaveCheckbox(BCheckBox* checkbox, const char* prefName)
{
	Prefs()->SetBoolPref(prefName, (checkbox->Value() == B_CONTROL_ON));
}


void PrefsDlog::CheckboxClicked(BMessage* message)
{
	BCheckBox* checkbox;
	status_t result = message->FindPointer("source", (void**) &checkbox);
	if (result != B_NO_ERROR)
		return;

	if (checkbox == showDatesControl)
		SaveCheckbox(checkbox, "showDates");
	else if (checkbox == showLinesControl)
		SaveCheckbox(checkbox, "showLines");
}


