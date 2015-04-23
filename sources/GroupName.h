/* GroupName.h */

#ifndef _H_GroupName_
#define _H_GroupName_

#include "DamnListableObject.h"
#include "string_slice.h"

class GroupName : public DamnListableObject {
public:
	GroupName(string_slice nameIn);
	bool        	isLessThan(const OrderedObject& otherObject);
	bool        	isEqualTo(const OrderedObject& otherObject);
	int         	DisplayHeight();
	void        	Draw(BRect rect, AATreeListView* view);
	void        	MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view);
	string_slice	Name();

protected:
	string_slice      	name;
	static float      	baseline;
	static const int  	displayHeight;
	static const float	nameLeft;
};


#endif
