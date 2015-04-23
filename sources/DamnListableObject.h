/* DamnListableObject.h */

#ifndef _H_DamnListableObject_
#define _H_DamnListableObject_

#include "ListableObject.h"

// stupid hack because Castle Yankee doesn't support operator overloading

class DamnListableObject : public ListableObject {
public:
	bool	operator<(const OrderedObject& otherObject)
				{ return this->isLessThan(otherObject); }
	bool	operator==(const OrderedObject& otherObject)
				{ return this->isEqualTo(otherObject); }

	// pure virtual
	virtual bool	isLessThan(const OrderedObject& otherObject) = 0;
	virtual bool	isEqualTo(const OrderedObject& otherObject) = 0;	
};


#endif
