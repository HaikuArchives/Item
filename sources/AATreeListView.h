/* AATreeListView.h */

#ifndef _H_AATreeListView_
#define _H_AATreeListView_

#include <View.h>

class ListableTree;
class ListableObject;

class AATreeListView : public BView {
public:
	AATreeListView(BRect frame, ListableTree* treeIn, const char* name = NULL,
               uint32 resizingMode = B_FOLLOW_ALL_SIDES,
               uint32 flags = 0);
	void           	SetTree(ListableTree* newTree);
	ListableTree*  	Tree();
	void           	InvalidateItem(ListableObject* item);
	virtual void   	SelectItem(ListableObject* item);
	virtual void   	DeselectItem(ListableObject* item);
	virtual void   	SelectAll();
	virtual void   	DeselectAll();
	void           	Select(int index);
	void           	Deselect(int index);
	void           	SelectFirst();
	int            	NumSelections();
	ListableObject*	SelectionAt(int index);
	void           	OpenSelections();
	void           	ScrollToSelection();
	int            	IndexOf(ListableObject* object);
	int            	NumObjects();
	void           	Draw(BRect updateRect);
	void           	MouseDown(BPoint point);
	void           	KeyDown(const char *bytes, int32 numBytes);
	void           	MouseMoved(BPoint point, uint32 transit, const BMessage *message);
	void           	TargetedByScrollView(BScrollView* scrollerIn);
	void           	AttachedToWindow();
	void           	FrameResized(float width, float height);
	void           	NumItemsChanged();
	void           	ItemDisplayChanged(ListableObject* item);
	void           	UpdateScrollBar();

protected:
	ListableTree*     	tree;
	BScrollView*      	scroller;
	static const float	SmallStepSize;
};


#endif
