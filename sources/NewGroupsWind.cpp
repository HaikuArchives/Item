/* NewGroupsWind.cpp */

#include "NewGroupsWind.h"
#include "NNTPApp.h"
#include "AllGroupsList.h"


NewGroupsWind::NewGroupsWind()
	: GroupsListWind(((NNTPApp*) be_app)->GetNewGroupsList(), "New Newsgroups", "newGroupsWind.frame")
{
}


