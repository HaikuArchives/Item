/* IntRangeTree.cpp */

#include "IntRangeTree.h"


IntRangeTree::IntRangeTree()
	: AATree()
{
}


IntRangeTree::~IntRangeTree()
{
	DeleteAllObjects();
}


void IntRangeTree::Insert(IntRange range)
{
	InsertObject(new IntRange(range));
}


IntRange IntRangeTree::RemoveOverlappingRange(IntRange range)
{
	IntRange foundRange;
	IntRange* removedRange = (IntRange*) RemoveObject(&range);
	if (removedRange) {
		foundRange = *removedRange;
		delete removedRange;
		}
	return foundRange;
}


IntRange IntRangeTree::FindOverlappingRange(IntRange range)
{
	IntRange* foundRange = (IntRange*) FindObject(&range);
	if (foundRange)
		return *foundRange;
	else
		return IntRange();	// invalid range
}


IntRange IntRangeTree::RangeAt(unsigned int index)
{
	IntRange result;
	IntRange* rangeObj = (IntRange*) ObjectAt(index);
	if (rangeObj)
		result = *rangeObj;
	return result;
}


