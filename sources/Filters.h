/* Filters.h */

#ifndef _H_Filters_
#define _H_Filters_

#include "ListableTree.h"
#include "string_slice.h"

class FilterGroup;

class Filters : public ListableTree {
public:
	Filters();
	~Filters();
	FilterGroup*	GetFilterGroup(string_slice groupName);
	FilterGroup*	GetGlobalFilters();
	void        	AddFilterGroup(FilterGroup* filterGroup);
	void        	Save();
	void        	Load();
	static void 	WriteFilterGroup(OrderedObject* object, void* data);

	static const char*	allGroupsName;
};


#endif
