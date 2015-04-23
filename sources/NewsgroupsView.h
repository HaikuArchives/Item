/* NewsgroupsView.h */

#ifndef _H_NewsgroupsView_
#define _H_NewsgroupsView_

#include "ScrolledListView.h"
#include "PrefsChangeClient.h"

class Newsrc;

class NewsgroupsView : public ScrolledListView, public PrefsChangeClient {
public:
	NewsgroupsView(BRect frame, Newsrc* newsrcIn);
	~NewsgroupsView();
	int  	NumItems();
	float	ItemHeight();
	void 	DrawItem(int32 itemIndex, BRect itemRect, bool selected);
	void 	OpenSelectedItem(int32 selectedIndex);
	void 	RemoveSelectedItem(int32 selectedIndex);
	bool 	CanRearrange();
	void 	ItemMoved(int32 oldIndex, int32 newIndex);
	void 	MouseMoved(BPoint point, uint32 transit, const BMessage* message);
	void 	PrefChanged(string_slice prefName);
	void 	SetupFont();

protected:
	Newsrc*               	newsrc;
	BFont                 	font;
	float                 	itemHeight;
	float                 	baseline;
	float                 	numArticlesWidth;
	float                 	groupNameLeft;
	static const float    	leftMargin;
	static const float    	separationEms;
	static const rgb_color	numArticlesColor;
	static const rgb_color	XtextColor;
	static const rgb_color	XbgndColor;
	static const rgb_color	XhiliteBgndColor;
	static const float    	XnumArticlesRight;
	static const float    	XgroupNameLeft;
	static const float    	Xbaseline;
};


#endif
