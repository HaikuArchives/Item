/* GroupsList.h */

#ifndef _H_GroupsList_
#define _H_GroupsList_

#include "ThreadOwner.h"
#include "string_slice.h"

class ListableTree;
class NNTPResponse;
class OrderedObject;
class GroupsListView;

class GroupsList : public BLocker, public ThreadOwner {
public:
	GroupsList();
	~GroupsList();
	void         	AddGroup(string_slice groupName);
	void         	SetResponse(NNTPResponse* newResponse);
	void         	AttachedToView(GroupsListView* viewIn);
	void         	DetachedFromView();
	ListableTree*	GetTree();

protected:
	ListableTree*   	tree;
	NNTPResponse*   	response;
	GroupsListView* 	view;
	static const int	fileReadUpdateQuantum;
	static const int	fileWriteUpdateQuantum;
};


#endif
