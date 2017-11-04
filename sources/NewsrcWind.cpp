/* NewsrcWind.cpp */

#include "NewsrcWind.h"
#include "NewsgroupsView.h"
#include "GetGroupStatsThread.h"
#include "NNTPApp.h"
#include "Preferences.h"
#include "qstring.h"
#include "Messages.h"
#include <MenuBar.h>
#include <Menu.h>
#include <MenuItem.h>
#include <ScrollView.h>
#include <ScrollBar.h>

const float NewsrcWind::defaultWidth = 300;
const float NewsrcWind::defaultHeight = 400;
const float NewsrcWind::defaultLeft = 50;
const float NewsrcWind::defaultTop = 50;


NewsrcWind::NewsrcWind(Newsrc* newsrc, string_slice fileName)	// assumes ownership
	: BWindow(BRect(0, 0, defaultWidth, defaultHeight), qstring(fileName).c_str(),
	          B_DOCUMENT_WINDOW, 0)
{
	// menu bar
	BMenuBar* menuBar = new BMenuBar(BRect(0, 0, 0, 0), "");
	BMenu* newsgroupMenu = new BMenu("Newsgroups");
	BMenuItem* filtersItem =
		new BMenuItem("Show Filters", new BMessage(ShowFiltersMessage), 'F');
	filtersItem->SetTarget(be_app_messenger);
	newsgroupMenu->AddItem(filtersItem);
	BMenuItem* groupsListItem =
		new BMenuItem("Show All Newsgroups", new BMessage(ShowGroupsListMessage));
	groupsListItem->SetTarget(be_app_messenger);
	newsgroupMenu->AddItem(groupsListItem);
	BMenuItem* prefsItem =
		new BMenuItem("Show Preferences", new BMessage(ShowPrefsMessage), ';');
	prefsItem->SetTarget(be_app_messenger);
	newsgroupMenu->AddItem(prefsItem);
	BMenuItem* aboutItem =
		new BMenuItem("About Item...", new BMessage(ShowAboutMessage));
	aboutItem->SetTarget(be_app_messenger);
	newsgroupMenu->AddItem(aboutItem);
	menuBar->AddItem(newsgroupMenu);
	AddChild(menuBar);

	// make the listView
	BRect listViewFrame(0, menuBar->Frame().Height() + 1,
	                    defaultWidth - B_V_SCROLL_BAR_WIDTH, defaultHeight);
	listView = new NewsgroupsView(listViewFrame, newsrc);

	// make the scrollView
	BScrollView* scrollView =
		new BScrollView("", listView, B_FOLLOW_ALL_SIDES, 0, false, true, B_NO_BORDER);
	AddChild(scrollView);

	// place it on the screen
	MoveTo(defaultLeft, defaultTop);
	BRect savedFrame = Prefs()->GetRectPref("newsrcWind.frame");
	if (savedFrame.IsValid()) {
		MoveTo(savedFrame.LeftTop());
		ResizeTo(savedFrame.Width(), savedFrame.Height());
		}

	// set up & show
	listView->MakeFocus();
	listView->Select(0);
	Show();

	// start the GetGroupStatsThread
	StartThread(new GetGroupStatsThread(newsrc, this));
}


NewsrcWind::~NewsrcWind()
{
	// notify the app
	((NNTPApp*) be_app)->WindowClosing(this);
}


void NewsrcWind::InvalidateNewsgroupAt(int index)
{
	Lock();
	listView->InvalidateItem(index);
	Unlock();
}


void NewsrcWind::Quit()
{
	// kill threads before taking down the window
	Hide();		// helps prevent deadlock
	KillOwnedThreads();
	BWindow::Quit();
}


void NewsrcWind::FrameMoved(BPoint screenPoint)
{
	BWindow::FrameMoved(screenPoint);
	SavePosition();
}


void NewsrcWind::FrameResized(float width, float height)
{
	BWindow::FrameResized(width, height);
	SavePosition();
}


void NewsrcWind::SavePosition()
{
	// don't do it if we're the only NewsgroupWind
	for (int i=0; ; i++) {
		BWindow* wind = be_app->WindowAt(i);
		if (wind == NULL)
			break;
		if (wind != this && dynamic_cast<NewsrcWind*>(wind) != NULL)
			return;
		}

	Prefs()->SetRectPref("newsrcWind.frame", Frame());
}


