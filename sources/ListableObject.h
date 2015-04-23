/* ListableObject.h */

#ifndef _H_ListableObject_
#define _H_ListableObject_

#include "OrderedObject.h"
#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <Rect.h>

class AATreeListView;

class ListableObject : public OrderedObject {
public:
	ListableObject();
	virtual int 	DisplayHeight();
	virtual void	Draw(BRect rect, AATreeListView* view);
	virtual void	MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view);
	virtual void	Open(AATreeListView* view);
	virtual bool	Selected();
	virtual void	SetSelected(bool newSelected);
	virtual void	SelectDown(uint32 modifiers, AATreeListView* view);
	virtual void	SelectUp(uint32 modifiers, AATreeListView* view);
	void        	DrawExpander(BView* view, BPoint bottomLeft, bool expanded, bool tracking = false);
	bool        	TrackExpander(BView* view, BPoint bottomLeft, bool expanded);

protected:
	bool                  	selected;
	static const float    	expanderHeight;
	static const float    	expanderWidth;
	static const rgb_color	expanderColor;
	static const rgb_color	expanderTrimColor;
};


#endif
