/* IntRange.cpp */

#include "IntRange.h"

IntRange IntRange::operator&(IntRange otherRange)
{
	int newMin = (min > otherRange.min ? min : otherRange.min);
	int newMax = (max < otherRange.max ? max : otherRange.max);
	return IntRange(newMin, newMax);
}

bool IntRange::operator<(const OrderedObject& otherObj)
{
	const IntRange& otherRange = *((IntRange*) &otherObj);
	return (max < otherRange.min);
}

bool IntRange::operator==(const OrderedObject& otherObj)
{
	const IntRange& otherRange = *((IntRange*) &otherObj);
	return (max >= otherRange.min && min <= otherRange.max);
}


