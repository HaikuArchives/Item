/* IntRangeTree.h */

#ifndef _H_IntRangeTree_
#define _H_IntRangeTree_

#include "AATree.h"
#include "IntRange.h"

class IntRangeTree : public AATree {
public:
	IntRangeTree();
	~IntRangeTree();
	void               	Insert(IntRange range);
	IntRange           	RemoveOverlappingRange(IntRange range);
	IntRange           	FindOverlappingRange(IntRange range);
	inline unsigned int	NumRanges();
	IntRange           	RangeAt(unsigned int index);
};

inline unsigned int IntRangeTree::NumRanges()
{
	return NumObjects();
}




#endif
