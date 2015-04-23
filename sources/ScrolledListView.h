/* ScrolledListView.h */

#ifndef _H_ScrolledListView_
#define _H_ScrolledListView_

#include <View.h>

class ScrolledListView : public BView {
public:
	ScrolledListView(BRect frame, const char *name,
                 uint32 resizingMode, uint32 flags);
	virtual int  	NumItems();
	virtual float	ItemHeight();
	virtual void 	DrawItem(int32 itemIndex, BRect itemRect, bool selected);
	virtual void 	OpenSelectedItem(int32 selectedIndex);
	virtual void 	RemoveSelectedItem(int32 selectedIndex);
	virtual void 	SelectionChanging(int32 newSelection, int32 oldSelection);
	virtual void 	ActivationChanging(bool toActive);
	virtual bool 	CanRearrange();
	virtual void 	ItemMoved(int32 oldIndex, int32 newIndex);
	void         	Select(int32 index);
	void         	ScrollToSelection();
	void         	InvalidateItem(int index);
	void         	Activate();
	void         	Deactivate();
	void         	Draw(BRect updateRect);
	void         	MouseDown(BPoint point);
	void         	KeyDown(const char *bytes, int32 numBytes);
	void         	MouseMoved(BPoint point, uint32 transit, const BMessage *message);
	void         	TargetedByScrollView(BScrollView* scrollerIn);
	void         	AttachedToWindow();
	void         	FrameResized(float width, float height);
	void         	NumItemsChanged();
	void         	UpdateScrollBar();
	void         	TrackRearrangement(BPoint point);
	int          	TrackInsertionStep(BPoint point, int prevInsertionIndex);
	void         	DrawItemAt(int32 index);

protected:
	int                   	selection;
	BScrollView*          	scroller;
	bool                  	active;
	static const rgb_color	insertionIndicatorColor;
};


#endif
