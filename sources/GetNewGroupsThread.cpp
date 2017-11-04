/* GetNewGroupsThread.cpp */

#include "GetNewGroupsThread.h"
#include "AllGroupsList.h"
#include "ConnectionGetter.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "NNTPApp.h"
#include "Task.h"
#include "Preferences.h"
#include "OSException.h"


GetNewGroupsThread::GetNewGroupsThread()
{
}


void GetNewGroupsThread::Action()
{
	Task task("Getting new groups...");

	try {

	// get the relevant times
	time_t lastUpdate = Prefs()->GetInt32Pref("lastGroupsUpdateTime");
	time_t curTime = time(NULL);

	// get a connection
	ConnectionGetter connectionGetter;
	NNTPConnection* connection = connectionGetter.Connection();
	if (connection == NULL)
		throw OSException("Couldn't get connection to get new groups.");
	task.GotConnection();

	// send the NEWGROUPS command
	// ignore the compiler warning about strftime(); the NEWGROUPS command
	// really does take only a two-digit year
	char command[128];
	struct tm* timeStruct = gmtime(&lastUpdate);
	strftime(command, 128, "NEWGROUPS %y%m%d %H%M%S GMT", timeStruct);
	status_t result = connection->SendCommand(command);
	if (result != B_NO_ERROR)
		throw OSException("Couldn't send command to get the new groups.", result);
	NNTPResponse* response;
	result = connection->GetResponse(&response);
	if (result != 231)
		throw OSException("An error occurred while getting the new groups.", result);
	response->NextLine();	// skip rest of response line

	// read the result
	GroupsList* groupsList = NULL;
	GroupsList* allGroupsList = NULL;
	for (; !response->AtEOF(); response->NextLine()) {
		string_slice groupName = response->NextField();
		if (groupName.length() == 0 || groupName == ".")	// check for ".", because NextField() doesn't deal with that like NextLine() does
			continue;

		// set up the groupsList (& allGroupsList) if we haven't done so already
		if (groupsList == NULL) {
			NNTPApp* app = (NNTPApp*) be_app;
			groupsList = app->GetNewGroupsList();
			groupsList->SetResponse(response);
			allGroupsList = app->GetAllGroupsList();
			app->OpenNewGroupsWind();
			}

		// add the group (but check if it's already there first)
		groupsList->AddGroup(groupName);
		allGroupsList->AddGroup(groupName);

		// update progress
		task.BumpProgress();
		}

	// update the pref
	Prefs()->SetInt32Pref("lastGroupsUpdateTime", curTime);
	}
	catch (DisplayableException& e) {
		e.Display();
		}
}


