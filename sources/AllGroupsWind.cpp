/* AllGroupsWind.cpp */

#include "AllGroupsWind.h"
#include "NNTPApp.h"
#include "AllGroupsList.h"


AllGroupsWind::AllGroupsWind()
	: GroupsListWind(((NNTPApp*) be_app)->GetAllGroupsList(), "All Newsgroups", "groupsListWind.frame")
{
}


