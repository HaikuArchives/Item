/* FiltersView.h */

#ifndef _H_FiltersView_
#define _H_FiltersView_

#include "AATreeListView.h"

#include <TextControl.h>

class Filter;

class FiltersView : public AATreeListView {
public:
	FiltersView(BRect frame, ListableTree* tree, const char* name = NULL,
               uint32 resizingMode = B_FOLLOW_ALL_SIDES,
               uint32 flags = 0);
	~FiltersView();
	void	EditFilterString(Filter* filter, BRect rect);
	void	FinishStringEdit();
	void	MessageReceived(BMessage* message);

protected:
	BTextControl*	stringEditor;
	Filter*      	editingFilter;
};


#endif
