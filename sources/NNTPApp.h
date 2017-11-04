/* NNTPApp.h */

#ifndef _H_NNTPApp_
#define _H_NNTPApp_

#include "ThreadOwner.h"
#include <Application.h>
#include <FilePanel.h>

class Filters;
class AllGroupsList;
class GroupsList;
class FiltersWind;
class GroupsListWind;
class PrefsDlog;

class NNTPApp : public BApplication, public ThreadOwner {
public:
	NNTPApp();
	~NNTPApp();
	void          	RefsReceived(BMessage* message);
	void          	ReadyToRun();
	void          	MessageReceived(BMessage* message);
	void          	OpenFiltersWind();
	void          	OpenAllGroupsWind();
	void          	OpenNewGroupsWind();
	void          	OpenPrefsWind();
	void          	WindowClosing(BWindow* wind);
	void          	ShowAppendArticlesPanel(BMessage* message);
	void          	ShowSaveArticlesPanel(BMessage* message);
	void          	ShowAbout();
	Filters*      	GetFilters();
	AllGroupsList*	GetAllGroupsList();
	GroupsList*   	GetNewGroupsList();
	bool          	SetupServer();
	void          	AppendArticles(BMessage* message);
	void          	SaveArticles(BMessage* message);
	void          	PostArticles(BMessage* message);

protected:
	Filters*       	filters;
	AllGroupsList* 	allGroupsList;
	GroupsList*    	newGroupsList;
	bool           	haveOpenedFile;
	FiltersWind*   	filtersWind;
	GroupsListWind*	allGroupsWind;
	GroupsListWind*	newGroupsWind;
	PrefsDlog*     	prefsWind;
	BFilePanel*    	appendArticlesPanel;
	BFilePanel*    	saveArticlesPanel;
};


#endif
