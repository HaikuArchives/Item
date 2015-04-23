/* GroupsListWind.h */

#ifndef _H_GroupsListWind_
#define _H_GroupsListWind_

#include "string_slice.h"
#include <Window.h>

class GroupsList;
class GroupsListView;

class GroupsListWind : public BWindow {
public:
	GroupsListWind(GroupsList* groupsList, const char* title, const char* framePrefNameIn);
	~GroupsListWind();

protected:
	GroupsListView*   	listView;
	const char*       	framePrefName;
	static const float	defaultWidth;
	static const float	defaultHeight;
	static const float	defaultLeft;
	static const float	defaultTop;
};


#endif
