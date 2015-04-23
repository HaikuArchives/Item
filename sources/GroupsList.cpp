/* GroupsList.cpp */

#include "GroupsList.h"
#include "ListableTree.h"
#include "NNTPResponse.h"
#include "GroupName.h"
#include "GroupsListView.h"
#include <Window.h>
#include <ScrollView.h>

const int GroupsList::fileReadUpdateQuantum = 1000;
const int GroupsList::fileWriteUpdateQuantum = 1000;


GroupsList::GroupsList()
	: tree(new ListableTree()), response(NULL), view(NULL)
{
}


GroupsList::~GroupsList()
{
	KillOwnedThreads();

	tree->DeleteAllObjects();
	delete tree;
	delete response;
}


void GroupsList::AddGroup(string_slice groupName)
{
	Lock();
	GroupName* group = new GroupName(groupName);
	bool hadGroup = (tree->FindObject(group) != NULL);
	if (!hadGroup)
		tree->InsertObject(group);
	else
		delete group;
	Unlock();

	// update window
	if (!hadGroup && view) {
		BWindow* wind = view->Window();
		if (wind && wind->Lock()) {
			view->NumItemsChanged();
			wind->Unlock();
			}
		}
}


void GroupsList::SetResponse(NNTPResponse* newResponse)
{
	delete response;
	response = newResponse;
}


void GroupsList::AttachedToView(GroupsListView* viewIn)
{
	view = viewIn;
}


void GroupsList::DetachedFromView()
{
	view = NULL;
}


ListableTree* GroupsList::GetTree()
{
	return tree;
}


