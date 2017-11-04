/* string_slice.cpp */

#include "string_slice.h"
#include "qstring.h"

bool string_slice::operator==(const char* str) const
{
	const char* thisP = start;
	const char* strP = str;
	while (true) {
		char strC = *strP++;

		// check for end
		if (thisP >= stopper)
			return (strC == 0);
		else if (strC == 0)
			return false;

		// compare
		if (*thisP++ != strC)
			return false;
		}
}


bool string_slice::operator==(string_slice other) const
{
	const char* thisP = start;
	const char* otherP = other.start;
	const char* otherStopper = other.stopper;
	while (true) {
		// check for end
		if (thisP >= stopper)
			return (otherP == otherStopper);
		else if (otherP >= otherStopper)
			return false;

		// compare
		if (*thisP++ != *otherP++)
			return false;
		}
}


bool string_slice::operator<(string_slice other) const
{
	const char* thisP = start;
	const char* otherP = other.start;
	const char* otherStopper = other.stopper;
	while (true) {
		// check for end
		if (thisP >= stopper)
			return (otherP != otherStopper);
		else if (otherP >= otherStopper)
			return false;

		// compare
		if (*thisP < *otherP)
			return true;
		else if (*thisP++ > *otherP++)
			return false;
		}
}


string_slice string_slice::substr(unsigned int firstChar, unsigned int numChars) const
{
	unsigned int len = stopper - start;
	if (firstChar > len)
		firstChar = len;
	if (numChars > len - firstChar)
		numChars = len - firstChar;

	return string_slice(start + firstChar, numChars);
}


void string_slice::eraseFirst(unsigned int numChars)
{
	unsigned int len = stopper - start;
	if (numChars > len)
		numChars = len;

	start += numChars;
}


void string_slice::resize(unsigned int newSize)
{
	if (newSize > (unsigned int) (stopper - start))
		return;		// not allowed

	stopper = start + newSize;
}


void string_slice::copy(char* dest, unsigned int numChars, unsigned int firstChar) const
{
	unsigned int len = stopper - start;
	if (firstChar > len)
		firstChar = len;
	if (numChars > len - firstChar)
		numChars = len - firstChar;

	memcpy(dest, start + firstChar, numChars);
}


int string_slice::asInt() const
{
	return qstring(*this).asInt();
}


double string_slice::asDouble() const
{
	return qstring(*this).asDouble();
}


bool string_slice::containsCI(string_slice other) const
{
	const char* p = start;
	for (; p<stopper; p++) {
		char thisC = *p;
		if (thisC >= 'A' && thisC <= 'Z')
			thisC += 'a' - 'A';
		char otherC = *other.start;
		if (otherC >= 'A' && otherC <= 'Z')
			otherC += 'a' - 'A';
		if (thisC == otherC) {
			const char* pp = p + 1;
			const char* otherP = other.start + 1;
			while (true) {
				if (otherP >= other.stopper)
					return true;
				else if (pp >= stopper) {
					// the section of "this" we're looking at is now shorter
					// than "other", and can't contain it
					return false;
					}
				thisC = *pp++;
				if (thisC >= 'A' && thisC <= 'Z')
					thisC += 'a' - 'A';
				otherC = *otherP++;
				if (otherC >= 'A' && otherC <= 'Z')
					otherC += 'a' - 'A';
				if (thisC != otherC)
					break;
				}
			}
		}
	return false;
}


bool string_slice::startsWith(string_slice other) const
{
	if (other.length() > length())
		return false;
	const char* p1 = start;
	const char* p2 = other.start;
	while (p2 < other.stopper) {
		if (*p1++ != *p2++)
			return false;
		}
	return true;
}


bool string_slice::endsWith(string_slice other) const
{
	unsigned long otherLen = other.length();
	if (otherLen > length())
		return false;
	const char* p1 = stopper - otherLen;
	const char* p2 = other.start;
	while (p2 < other.stopper) {
		if (*p1++ != *p2++)
			return false;
		}
	return true;
}


string_slice string_slice::trim() const
{
	// sanity clause
	if (start == stopper)
		return string_slice();

	// trim the beginning
	const char* newStart = start;
	for (; newStart < stopper; ++newStart) {
		char c = *newStart;
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
			break;
		}

	// trim the end
	const char* last = stopper - 1;
	for (; last >= newStart; --last) {
		char c = *last;
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
			break;
		}

	return string_slice(newStart, last + 1);
}


string_slice string_slice::separate(string_slice separator, string_slice* remainder) const
{
	// search for the separator
	for (const char* p = start; p < stopper; ++p) {
		if (string_slice(p, stopper).startsWith(separator)) {
			// found it
			const char* origStart = start;	// in case "remainder == this"
			if (remainder)
				*remainder = string_slice(p + separator.length(), stopper);
			return string_slice(origStart, p);
			}
		}

	// if it wasn't there, this is the last value
	string_slice value = *this;		// in case "remainder == this"
	if (remainder)
		*remainder = string_slice();
	return value;
}



