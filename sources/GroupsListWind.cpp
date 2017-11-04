/* GroupsListWind.cpp */

#include "GroupsListWind.h"
#include "GroupsList.h"
#include "GroupsListView.h"
#include "NNTPApp.h"
#include "Preferences.h"
#include <ScrollView.h>

const float GroupsListWind::defaultWidth = 300;
const float GroupsListWind::defaultHeight = 400;
const float GroupsListWind::defaultLeft = 500;
const float GroupsListWind::defaultTop = 100;


GroupsListWind::GroupsListWind(GroupsList* groupsList, const char* title, const char* framePrefNameIn)
	: BWindow(BRect(defaultLeft, defaultTop, defaultLeft + defaultWidth, defaultTop + defaultHeight),
	          title, B_DOCUMENT_WINDOW, 0),
	  framePrefName(framePrefNameIn)
{
	// make the listView
	BRect listViewFrame(0, 0,
	                    defaultWidth - B_V_SCROLL_BAR_WIDTH, defaultHeight);
	listView = new GroupsListView(listViewFrame, groupsList);

	// make the scrollView
	BScrollView* scrollView =
		new BScrollView("", listView, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER);
	AddChild(scrollView);

	// adjust the position
	BRect savedFrame = Prefs()->GetRectPref(framePrefName);
	if (savedFrame.IsValid()) {
		MoveTo(savedFrame.LeftTop());
		ResizeTo(savedFrame.Width(), savedFrame.Height());
		}

	listView->MakeFocus();
	Show();
}


GroupsListWind::~GroupsListWind()
{
	Prefs()->SetRectPref(framePrefName, Frame());
	((NNTPApp*) be_app)->WindowClosing(this);
}


