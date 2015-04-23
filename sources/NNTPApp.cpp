/* NNTPApp.cpp */

#include "NNTPApp.h"
#include "Filters.h"
#include "AllGroupsList.h"
#include "FiltersWind.h"
#include "AllGroupsWind.h"
#include "NewGroupsWind.h"
#include "PrefsDlog.h"
#include "OpenNewsrcThread.h"
#include "OpenGroupThread.h"
#include "SaveArticleThread.h"
#include "PostArticlesThread.h"
#include "GetNewGroupsThread.h"
#include "ServerDlog.h"
#include "TasksWind.h"
#include "Messages.h"
#include "Prefs.h"
#include <Entry.h>
#include <Message.h>
#include <File.h>
#include <FilePanel.h>
#include <Screen.h>
#include <Alert.h>
#include <Path.h>
#include <NodeInfo.h>



NNTPApp::NNTPApp()
	: BApplication("application/x-vnd.Folta-NNTPTest"),
	  allGroupsList(NULL), newGroupsList(NULL), haveOpenedFile(false),
	  filtersWind(NULL), allGroupsWind(NULL), newGroupsWind(NULL), prefsWind(NULL),
	  appendArticlesPanel(NULL), saveArticlesPanel(NULL)
{
	// set up the filters
	filters = new Filters();
}


NNTPApp::~NNTPApp()
{
	delete filters;
	delete allGroupsList;
	delete newGroupsList;
}


void NNTPApp::RefsReceived(BMessage* message)
{
	if (!SetupServer()) {
		Quit();
		return;
		}

	entry_ref ref;
	for (int i=0; ; i++) {
		if (message->FindRef("refs", i, &ref) != B_NO_ERROR)
			break;
		OpenNewsrcThread* thread = new OpenNewsrcThread(&ref);
		thread->Go();
		}

	haveOpenedFile = true;
}


void NNTPApp::ReadyToRun()
{
	// setup server
	if (!SetupServer()) {
		Quit();
		return;
		}

	// open "My Newsgroups"
	if (!haveOpenedFile) {
		// open default "My Newsgroups" file
		static const char* newsrcPath = "/boot/home/My Newsgroups";
		// open the file in order to create it if it doesn't exist
		BEntry newsrcEntry(newsrcPath);
		if (!newsrcEntry.Exists()) {
			BFile newsrcFile(&newsrcEntry, B_READ_WRITE | B_CREATE_FILE);
			// set the MIME type
			BNode node(&newsrcEntry);
			BNodeInfo nodeInfo(&node);
			nodeInfo.SetType("text/x-newsrc");
			}
		// open the file in a window
		entry_ref newsrcRef;
		newsrcEntry.GetRef(&newsrcRef);
		OpenNewsrcThread* thread = new OpenNewsrcThread(&newsrcRef);
		thread->Go();
		}

	// check for new groups
	int32 lastGroupsUpdateTime = Prefs()->GetInt32Pref("lastGroupsUpdateTime");
	if (lastGroupsUpdateTime != 0) {
		GetNewGroupsThread* newGroupsThread = new GetNewGroupsThread();
		newGroupsThread->Go();
		}
	else {
		// otherwise, get the full groups list
		// Set "lastGroupsUpdateTime" just in case there was an old version,
		// cause if we don't, it'll never get set and the allGroupsList will
		// be read on startup every time.  We set it to zero to make sure
		// any new groups created since the full group list was created will
		// show up.
		Prefs()->SetInt32Pref("lastGroupsUpdateTime", 0);
		GetAllGroupsList();
		}

//*** test
//*** OpenFiltersWind();

//*** test
/***
	OpenGroupThread* thread = new OpenGroupThread("rec.audio.pro");
	thread->Go();
***/
}


void NNTPApp::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case ShowFiltersMessage:
			OpenFiltersWind();
			break;

		case ShowGroupsListMessage:
			OpenAllGroupsWind();
			break;

		case ShowPrefsMessage:
			OpenPrefsWind();
			break;

		case ShowAboutMessage:
			ShowAbout();
			break;

		case AppendArticlesMessage:
			AppendArticles(message);
			break;

		case SaveArticlesMessage:
			SaveArticles(message);
			break;

		case PostArticlesMessage:
			PostArticles(message);
			break;

		default:
			BApplication::MessageReceived(message);
			break;
		}
}


void NNTPApp::OpenFiltersWind()
{
	if (filtersWind)
		filtersWind->Activate();
	else
		filtersWind = new FiltersWind();
}


void NNTPApp::OpenAllGroupsWind()
{
	if (allGroupsWind)
		allGroupsWind->Activate();
	else
		allGroupsWind = new AllGroupsWind();
}


void NNTPApp::OpenNewGroupsWind()
{
	if (newGroupsWind)
		newGroupsWind->Activate();
	else
		newGroupsWind = new NewGroupsWind();
}


void NNTPApp::OpenPrefsWind()
{
	if (prefsWind)
		prefsWind->Activate();
	else
		prefsWind = new PrefsDlog();
}


void NNTPApp::WindowClosing(BWindow* wind)
{
	// manage owned windows
	if (wind == filtersWind)
		filtersWind = NULL;
	else if (wind == allGroupsWind)
		allGroupsWind = NULL;
	else if (wind == prefsWind)
		prefsWind = NULL;
	else if (appendArticlesPanel != NULL && wind == appendArticlesPanel->Window())
		appendArticlesPanel = NULL;
	else if (saveArticlesPanel != NULL && wind == saveArticlesPanel->Window())
		saveArticlesPanel = NULL;

	// quit if all windows are closed
	int numWindows = CountWindows();
	if (TasksWind::HaveTasksWind())
		numWindows -= 1;	// ignore tasks window
	if (appendArticlesPanel)
		numWindows -= 1;	// ignore appendArticlesPanel
	if (saveArticlesPanel)
		numWindows -= 1;	// ignore saveArticlesPanel
	if (numWindows <= 1)
		Quit();
}


void NNTPApp::ShowAppendArticlesPanel(BMessage* message)
{
	// create the panel if it doesn't yet exist
	if (appendArticlesPanel == NULL) {
		// make the panel, having it show the saved directory
/***
		entry_ref* appendDir = Prefs()->GetRefPref("savedArticlesDir");
		appendArticlesPanel =
			new BFilePanel(B_OPEN_PANEL, NULL, appendDir, B_FILE_NODE, false);
***/
		appendArticlesPanel =
			new BFilePanel(B_OPEN_PANEL, NULL, NULL, B_FILE_NODE, false);
		const char* savedArticlesDirPath = Prefs()->GetStringPref("savedArticlesDir");
		if (savedArticlesDirPath)
			appendArticlesPanel->SetPanelDirectory(savedArticlesDirPath);

		// restore to saved position
		BWindow* panelWind = appendArticlesPanel->Window();
		BRect savedFrame = Prefs()->GetRectPref("savedArticlesPanelFrame");
		if (savedFrame.IsValid()) {
			if (BScreen(panelWind).Frame().Contains(savedFrame)) {
				panelWind->MoveTo(savedFrame.LeftTop());
				panelWind->ResizeTo(savedFrame.Width(), savedFrame.Height());
				}
			}

		// set the title
		panelWind->SetTitle("Append Article To:");
		}

	// set it up and show it
	message->what = AppendArticlesMessage;
	appendArticlesPanel->SetMessage(message);
	appendArticlesPanel->Show();
}


void NNTPApp::ShowSaveArticlesPanel(BMessage* message)
{
	// create the panel if it doesn't yet exist
	if (saveArticlesPanel == NULL) {
		// make the panel, having it show the saved directory
		saveArticlesPanel =
			new BFilePanel(B_SAVE_PANEL, NULL, NULL, B_FILE_NODE, false);
		const char* savedArticlesDirPath = Prefs()->GetStringPref("savedArticlesDir");
		if (savedArticlesDirPath)
			saveArticlesPanel->SetPanelDirectory(savedArticlesDirPath);

		// restore to saved position
		BWindow* panelWind = saveArticlesPanel->Window();
		BRect savedFrame = Prefs()->GetRectPref("savedArticlesPanelFrame");
		if (savedFrame.IsValid()) {
			if (BScreen(panelWind).Frame().Contains(savedFrame)) {
				panelWind->MoveTo(savedFrame.LeftTop());
				panelWind->ResizeTo(savedFrame.Width(), savedFrame.Height());
				}
			}

		// set the title
		panelWind->SetTitle("Save Article To:");
		}

	// set it up and show it
	saveArticlesPanel->SetSaveText(message->FindString("subjects"));
	message->what = SaveArticlesMessage;
	saveArticlesPanel->SetMessage(message);
	saveArticlesPanel->Show();
}


void NNTPApp::ShowAbout()
{
	static const char* aboutText =
		"Item\nA newsreader\n© 1999, 2000 Steve Folta\nBuild date: ";

	qstring msg = aboutText;
	msg += __DATE__ " " __TIME__;
	BAlert* alert = new BAlert("About Item", msg.c_str(), "OK");
	alert->Go();
}


Filters* NNTPApp::GetFilters()
{
	return filters;
}


AllGroupsList* NNTPApp::GetAllGroupsList()
{
	if (allGroupsList == NULL)
		allGroupsList = new AllGroupsList();
	return allGroupsList;
}


GroupsList* NNTPApp::GetNewGroupsList()
{
	if (newGroupsList == NULL)
		newGroupsList = new GroupsList();
	return newGroupsList;
}


bool NNTPApp::SetupServer()
{
	if (Prefs()->GetStringPref("nntpServer")[0] == 0) {
		ServerDlog* serverDlog = new ServerDlog();
		return serverDlog->Go();
		}
	else
		return true;
}


void NNTPApp::AppendArticles(BMessage* message)
{
	// get the data out of the message
	entry_ref ref;
	status_t result = message->FindRef("refs", &ref);
	if (result != B_NO_ERROR)
		return;

	// take this opportunity to set the "savedArticlesDir" and
	// "savedArticlesPanelFrame" prefs
	BEntry entry(&ref);
	entry.GetParent(&entry);
/***
	entry_ref dirRef;
	entry.GetRef(&dirRef);
	Prefs()->SetRefPref("savedArticlesDir", &dirRef);
***/
	BPath dirPath;
	entry.GetPath(&dirPath);
	Prefs()->SetStringPref("savedArticlesDir", dirPath.Path());
	if (appendArticlesPanel)
		Prefs()->SetRectPref("savedArticlesPanelFrame", appendArticlesPanel->Window()->Frame());

	// spawn threads to do the work
	(new SaveArticleThread(message))->Go();
}


void NNTPApp::SaveArticles(BMessage* message)
{
	// get the data out of the message
	entry_ref dirRef;
	status_t result = message->FindRef("directory", &dirRef);
	if (result != B_NO_ERROR)
		return;
	BDirectory dir(&dirRef);
	BEntry entry(&dir, message->FindString("name"));
	entry_ref ref;
	entry.GetRef(&ref);

	// add the "refs" entry to the message
	message->AddRef("refs", &ref);

	// create the file, delete existing data, set the MIME type
	{
		BFile file(&ref, B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
		BNodeInfo(&file).SetType("text/plain");
	}

	// take this opportunity to set the "savedArticlesDir" and
	// "savedArticlesPanelFrame" prefs
	BEntry dirEntry(&dirRef);
	BPath dirPath;
	dirEntry.GetPath(&dirPath);
	Prefs()->SetStringPref("savedArticlesDir", dirPath.Path());
	if (saveArticlesPanel)
		Prefs()->SetRectPref("savedArticlesPanelFrame", saveArticlesPanel->Window()->Frame());

	// spawn threads to do the work
	(new SaveArticleThread(message))->Go();
}


void NNTPApp::PostArticles(BMessage* message)
{
	status_t result;

	// file the list of file refs
	BList* fileRefs = new BList();
	for (int i=0; ; i++) {
		entry_ref ref;
		result = message->FindRef("refs", i, &ref);
		if (result != B_NO_ERROR)
			break;
		fileRefs->AddItem(new entry_ref(ref));
		}
	if (fileRefs->CountItems() == 0)
		return;

	// post
	StartThread(new PostArticlesThread(fileRefs));
}


