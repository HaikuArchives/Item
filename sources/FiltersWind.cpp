/* FiltersWind.cpp */

#include "FiltersWind.h"
#include "NNTPApp.h"
#include "FiltersView.h"
#include "Filters.h"
#include "Messages.h"
#include "Preferences.h"
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <ScrollView.h>

const float FiltersWind::defaultWidth = 400;
const float FiltersWind::defaultHeight = 400;
const float FiltersWind::defaultLeft = 500;
const float FiltersWind::defaultTop = 100;


FiltersWind::FiltersWind()
	: BWindow(BRect(defaultLeft, defaultTop, defaultLeft + defaultWidth, defaultTop + defaultHeight),
	          "Filters", B_DOCUMENT_WINDOW, 0)
{
	// menu bar
	BMenuBar* menuBar = new BMenuBar(BRect(0, 0, 0, 0), "");
	BMenu* filtersMenu = new BMenu("Filters");
	BMenuItem* newFilterItem =
		new BMenuItem("New Filter", new BMessage(NewFilterMessage), 'N');
	filtersMenu->AddItem(newFilterItem);
	BMenuItem* newGroupItem =
		new BMenuItem("New Group", new BMessage(NewGroupMessage));
	filtersMenu->AddItem(newGroupItem);
	menuBar->AddItem(filtersMenu);
	AddChild(menuBar);

	// make the listView
	BRect listViewFrame(0, menuBar->Frame().Height() + 1,
	                    defaultWidth - B_V_SCROLL_BAR_WIDTH, defaultHeight);
	listView = new FiltersView(listViewFrame, ((NNTPApp*) be_app)->GetFilters());

	// make the scrollView
	BScrollView* scrollView =
		new BScrollView("", listView, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER);
	AddChild(scrollView);

	// set the listView as the target of menu messages
	// --need to do this *after* the listView is in the view hierarchy, for some
	//   reason (prob'ly cause it doesn't have a BLooper yet otherwise)
	newFilterItem->SetTarget(listView);
	newGroupItem->SetTarget(listView);

	// adjust the position
	BRect savedFrame = Prefs()->GetRectPref("filterWindFrame");
	if (savedFrame.IsValid()) {
		MoveTo(savedFrame.LeftTop());
		ResizeTo(savedFrame.Width(), savedFrame.Height());
		}

	listView->MakeFocus();
	Show();
}


FiltersWind::~FiltersWind()
{
	Prefs()->SetRectPref("filterWindFrame", Frame());
	((NNTPApp*) be_app)->WindowClosing(this);
}


void FiltersWind::MenusBeginning()
{
	BMenuBar* menuBar = KeyMenuBar();
	menuBar->FindItem(NewFilterMessage)->SetEnabled(listView->NumSelections() > 0);
}


