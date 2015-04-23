/* ListableTree.h */

#ifndef _H_ListableTree_
#define _H_ListableTree_

#include "AATree.h"

class ListableObject;

class ListableTree : public AATree {
public:
	ListableTree();
	int            	DisplayHeight();
	int            	IndexForY(int y);
	int            	YAtIndex(int index);
	int            	YForObject(OrderedObject* object);
	int            	NumSelections();
	ListableObject*	SelectedObjectAt(int index);
	ListableObject*	FirstSelectedObject();
	ListableObject*	LastSelectedObject();
	void           	SelectObject(ListableObject* object);
	void           	DeselectObject(ListableObject* object);
	void           	Select(int index);
	void           	Deselect(int index);
	void           	SelectAllObjects();
	void           	DeselectAllObjects();
	void           	AllHeightsChanged();
	AATreeNode*    	MakeNode(OrderedObject* object);
	virtual void   	AdjustNode(AATreeNode* nodeIn);
	void           	SelectAllAt(AATreeNode* nodeIn);
	void           	DeselectAllAt(AATreeNode* nodeIn);
	void           	AdjustAllAt(AATreeNode* nodeIn);
};


#endif
