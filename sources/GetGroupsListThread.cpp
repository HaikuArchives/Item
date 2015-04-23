/* GetGroupsListThread.cpp */

#include "GetGroupsListThread.h"
#include "AllGroupsList.h"
#include "ConnectionGetter.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "NNTPApp.h"
#include "Task.h"
#include "Prefs.h"
#include "Error.h"

const int GetGroupsListThread::progressQuantum = 20;


GetGroupsListThread::GetGroupsListThread(AllGroupsList* groupsListIn)
	: groupsList(groupsListIn)
{
}


void GetGroupsListThread::Action()
{
	Task task("Getting the list of all newsgroups...");
	time_t curTime = time(NULL);

	// get a connection
	ConnectionGetter connectionGetter;
	NNTPConnection* connection = connectionGetter.Connection();
	if (connection == NULL) {
		Error::ShowError("Couldn't get connection to read all groups.");
		return;
		}
	task.GotConnection();

	// send the LIST command
	status_t result = connection->SendCommand("LIST");
	if (result != B_NO_ERROR) {
		Error::ShowError("Couldn't send command to get groups list.", result);
		return;
		}
	NNTPResponse* response;
	result = connection->GetResponse(&response);
	if (result != 215) {
		Error::ShowError("An error occurred while getting the groups list.", result);
		return;
		}
	groupsList->SetResponse(response);
	response->NextLine();	// skip rest of response line

	// read the result
	int bumpProgressAfter = progressQuantum;
	for (; !response->AtEOF(); response->NextLine()) {
		string_slice groupName = response->NextField();
		if (groupName.length() == 0 || groupName == ".")	// check for ".", because NextField() doesn't deal with that like NextLine() does
			continue;

		// add the group (but check if it's already there first)
		groupsList->AddGroup(groupName);

		// update progress
		--bumpProgressAfter;
		if (bumpProgressAfter <= 0) {
			task.BumpProgress();
			bumpProgressAfter = progressQuantum;
			}
		}

	groupsList->Dirtify();

	// update the pref
	Prefs()->SetInt32Pref("lastGroupsUpdateTime", curTime);

	// automatically pop the groups list window open
	((NNTPApp*) be_app)->OpenAllGroupsWind();
}


