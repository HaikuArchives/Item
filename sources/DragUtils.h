/* DragUtils.h */

#ifndef _H_DragUtils_
#define _H_DragUtils_

#include "string_slice.h"

class BList;
class BMessage;

class DragUtils {
public:
	static BList*	GetDraggedGroups(const BMessage* message);
	static bool  	HasNewsgroups(const BMessage* message);
	static bool  	IsGroupName(string_slice name);
};


#endif
