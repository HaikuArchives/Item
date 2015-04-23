/* FilterGroup.h */

#ifndef _H_FilterGroup_
#define _H_FilterGroup_

#include "DamnListableObject.h"
#include "string_slice.h"

class qstring;
class Filter;
class Article;
class FiltersView;

class FilterGroup : public DamnListableObject {
public:
	FilterGroup(string_slice groupNameIn);
	virtual~FilterGroup();
	void        	ApplyFilters(Article* article, string_slice subject);
	void        	AddFilter(Filter* filter);
	void        	EditFilterString(Filter* filter, BRect rect, FiltersView* view);
	string_slice	GroupName() const;
	int         	NumFilters();
	Filter*     	FilterAt(int index);
	bool        	isLessThan(const OrderedObject& otherObject);
	bool        	isEqualTo(const OrderedObject& otherObject);
	int         	DisplayHeight();
	void        	Draw(BRect rect, AATreeListView* view);
	void        	MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view);

protected:
	qstring*              	groupName;
	BList*                	filters;
	bool                  	expanded;
	static const float    	groupHeight;
	static const float    	expanderLeft;
	static const float    	itemLeft;
	static const float    	nameLeft;
	static const float    	filterHeight;
	static const float    	filterIndent;
	static const float    	baseline;
	static const rgb_color	bgndColor;
	static const rgb_color	hiliteBgndColor;
};


#endif
