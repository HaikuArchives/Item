/* GroupsListView.h */

#ifndef _H_GroupsListView_
#define _H_GroupsListView_

#include "AATreeListView.h"

class GroupsList;

class GroupsListView : public AATreeListView {
public:
	GroupsListView(BRect frame, GroupsList* groupsListIn);
	~GroupsListView();
	void	Draw(BRect updateRect);
	void	StartDrag(BRect itemRect);

protected:
	GroupsList*	groupsList;
};


#endif
