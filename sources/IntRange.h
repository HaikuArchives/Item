/* IntRange.h */

#ifndef _H_IntRange_
#define _H_IntRange_

#include "OrderedObject.h"

class IntRange : public OrderedObject {
public:
	IntRange(int minIn, int maxIn)
		: min(minIn), max(maxIn) {}
	IntRange(int val)
		: min(val), max(val) {}
	IntRange()
		: min(0), max(-1) {}

	bool    	IsValid() { return (min <= max); }
	IntRange	operator&(IntRange otherRange);		// intersection
	int     	Size() { return max - min + 1; }

	// comparisons
	// NOTE:  these find OVERLAPPING ranges, not identical ones
	bool	operator<(const OrderedObject& otherRange);
	bool	operator==(const OrderedObject& otherRange);

	int	min, max;
};


#endif
