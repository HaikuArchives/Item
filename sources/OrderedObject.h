/* OrderedObject.h */

#ifndef _H_OrderedObject_
#define _H_OrderedObject_

class OrderedObject {
public:
	virtual bool	operator<(const OrderedObject& otherObject) = 0;
	virtual bool	operator==(const OrderedObject& otherObject) = 0;

	bool	operator!=(const OrderedObject& otherObject)
				{ return !(*this == otherObject); }
};


#endif
