/* ArticleSet.cpp */

#include "ArticleSet.h"
#include "IntRangeTree.h"
#include "qstring.h"
#include <stdio.h>



ArticleSet::ArticleSet()
{
	tree = new IntRangeTree();
}


ArticleSet::~ArticleSet()
{
	delete tree;
}


void ArticleSet::AddArticle(int articleNo)
{
	if (tree->FindOverlappingRange(IntRange(articleNo)).IsValid())
		return;		// already there

	// get rid of adjacent ranges, and add merged range
	IntRange mergedRange(articleNo);
	IntRange leftRange = tree->RemoveOverlappingRange(IntRange(articleNo - 1));
	if (leftRange.IsValid())
		mergedRange.min = leftRange.min;
	IntRange rightRange = tree->RemoveOverlappingRange(IntRange(articleNo + 1));
	if (rightRange.IsValid())
		mergedRange.max = rightRange.max;
	tree->Insert(mergedRange);
}


void ArticleSet::AddRange(IntRange range)
{
	// get rid of all overlapping & adjacent ranges, merging them into "range"
	IntRange adjacentFinderRange(range.min - 1, range.max + 1);
	while (true) {
		IntRange existingRange = tree->RemoveOverlappingRange(adjacentFinderRange);
		if (!existingRange.IsValid())
			break;
		if (existingRange.min < range.min)
			range.min = existingRange.min;
		if (existingRange.max > range.max)
			range.max = existingRange.max;
		}

	// add the merged range
	tree->Insert(range);
}


void ArticleSet::RemoveArticle(int articleNo)
{
	// get rid of the range containing it
	IntRange removedRange = tree->RemoveOverlappingRange(IntRange(articleNo));
	if (!removedRange.IsValid())
		return;

	// add back two new ranges
	IntRange leftRange(removedRange.min, articleNo - 1);
	if (leftRange.IsValid())
		tree->Insert(leftRange);
	IntRange rightRange(articleNo + 1, removedRange.max);
	if (rightRange.IsValid())
		tree->Insert(rightRange);
}


void ArticleSet::RemoveRange(IntRange range)
{
	// get rid of all overlapping ranges, splitting them if necessary
	while (true) {
		IntRange removedRange = tree->RemoveOverlappingRange(range);
		if (!removedRange.IsValid())
			return;

		// left split
		IntRange leftRange(removedRange.min, range.min - 1);
		if (leftRange.IsValid())
			tree->Insert(leftRange);
		IntRange rightRange(range.max + 1, removedRange.max);
		if (rightRange.IsValid())
			tree->Insert(rightRange);
		}
}


bool ArticleSet::ContainsArticle(int articleNo)
{
	return tree->FindOverlappingRange(IntRange(articleNo)).IsValid();
}


void ArticleSet::AddArticles(string_slice str)
{
	const char* p = str.begin();
	const char* stopper = str.end();
	const char* numStart = p;
	for (; p<stopper; p++) {
		if (*p == ',') {
			AddArticle(string_slice(numStart, p).asInt());
			numStart = p + 1;
			}
		else if (*p == '-') {
			int firstArticle = string_slice(numStart, p).asInt();
			p++;	// skip hyphen
			// read the last article number
			numStart = p;
			while (p < stopper && *p != ',')
				p++;
			int lastArticle = string_slice(numStart, p).asInt();
			// add the range
			IntRange range(firstArticle, lastArticle);
			if (range.IsValid())
				AddRange(range);
			// set up for next go-round
			numStart = p + 1;
			}
		}
	// add the final article
	if (numStart < stopper)
		AddArticle(string_slice(numStart, stopper).asInt());
}


qstring* ArticleSet::AsString()
{
	char	numStr[128];

	qstring* str = new qstring;
	bool needsComma = false;
	unsigned int numRanges = tree->NumObjects();
	for (unsigned int i=0; i<numRanges; i++) {
		IntRange range = *(IntRange*) tree->ObjectAt(i);

		if (needsComma)
			*str += ',';
		needsComma = true;

		if (range.min == range.max)
			sprintf(numStr, "%d", range.min);
		else
			sprintf(numStr, "%d-%d", range.min, range.max);
		*str += numStr;
		}
	return str;
}


void ArticleSet::MakeEmpty()
{
	tree->DeleteAllObjects();
}


void ArticleSet::Incorporate(ArticleSet* otherSet)
{
	int numRanges = otherSet->NumRanges();
	for (int rangeIdx = 0; rangeIdx<numRanges; rangeIdx++)
		AddRange(otherSet->RangeAt(rangeIdx));
}


unsigned int ArticleSet::NumRanges()
{
	return tree->NumRanges();
}


IntRange ArticleSet::RangeAt(unsigned int index)
{
	return tree->RangeAt(index);
}


int ArticleSet::NumArticles()
{
	int numArticles = 0;
	int numRanges = NumRanges();
	for (int i=0; i<numRanges; i++)
		numArticles += RangeAt(i).Size();
	return numArticles;
}


int ArticleSet::NumArticlesIn(IntRange inRange)
{
	int numArticles = 0;
	int numRanges = NumRanges();
	for (int i=0; i<numRanges; i++) {
		IntRange range = RangeAt(i) & inRange;
		if (range.IsValid())
			numArticles += range.Size();
		}
	return numArticles;
}


