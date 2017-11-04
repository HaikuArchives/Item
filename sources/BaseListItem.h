/* BaseListItem.h */

#ifndef _H_BaseListItem_
#define _H_BaseListItem_

#include <ListItem.h>

class Newsgroup;

class BaseListItem : public BListItem {
public:
	BaseListItem(bool isSubItem);
	virtual bool	Open(Newsgroup* newsgroup);
	virtual void	Mark(Newsgroup* newsgroup);
	virtual void	Unmark(Newsgroup* newsgroup);
	void        	SetupColors(BView* view, BRect itemRect);

protected:
	static const rgb_color	HiliteColor;
	static const rgb_color	TextColor;
	static const float    	Baseline;
};


#endif
