/* qstring.cpp */

#include "qstring.h"
#include <stdio.h>
#include <stdlib.h>

static const int initialCapacity = 80;
static const int capacitySlop = 80;


qstring::qstring()
	: chars(new char[initialCapacity]), len(0), capacity(initialCapacity)
{
}


qstring::qstring(const char* str)
{
	len = strlen(str);
	capacity = len + capacitySlop;
	chars = new char[capacity];
	strcpy(chars, str);
}


qstring::qstring(string_slice slice)
{
	len = slice.length();
	capacity = len + capacitySlop;
	chars = new char[capacity];
	memcpy(chars, slice.begin(), len);
}


qstring::qstring(int val)
{
	char str[128];
	sprintf(str, "%d", val);
	len = strlen(str);
	capacity = len + capacitySlop;
	chars = new char[capacity];
	strcpy(chars, str);
}


qstring::qstring(const qstring& str)
{
	len = str.len;
	capacity = len + capacitySlop;
	chars = new char[capacity];
	memcpy(chars, str.chars, len);
}


qstring::~qstring()
{
	delete chars;
}


qstring& qstring::operator=(const char* str)
{
	len = strlen(str);
	if (len > capacity)
		reserve(len + capacitySlop, false);
	strcpy(chars, str);
	return *this;
}


qstring& qstring::operator=(string_slice slice)
{
	int newLen = slice.length();
	if (newLen > capacity)
		reserve(newLen + capacitySlop, false);
	memcpy(chars, slice.begin(), newLen);
	len = newLen;
	return *this;
}


qstring& qstring::operator=(const qstring& str)
{
	len = str.len;
	if (len > capacity)
		reserve(len + capacitySlop, false);
	memcpy(chars, str.chars, len);
	return *this;
}


qstring& qstring::operator+=(char c)
{
	if (len == capacity)
		reserve(len + capacitySlop);
	chars[len] = c;
	len++;
	return *this;
}


qstring& qstring::operator+=(const char* str)
{
	int argLen = strlen(str);
	if (len + argLen + 1 > capacity)
		reserve(len + argLen + capacitySlop);
	strcpy(&chars[len], str);
	len += argLen;
	return *this;
}


qstring& qstring::operator+=(string_slice slice)
{
	int argLen = slice.length();
	if (len + argLen > capacity)
		reserve(len + argLen + capacitySlop);
	memcpy(&chars[len], slice.begin(), argLen);
	len += argLen;
	return *this;
}


qstring& qstring::operator+=(int val)
{
	char str[128];
	sprintf(str, "%d", val);
	return *this += str;
}


qstring& qstring::operator+=(const qstring& str)
{
	if (len + str.len + 1 > capacity)
		reserve(len + str.len + capacitySlop);
	memcpy(&chars[len], str.chars, str.len);
	len += str.len;
	return *this;
}


qstring& qstring::append(const char* str, int n)
{
	if (len + n + 1 > capacity)
		reserve(len + n + capacitySlop);
	memcpy(&chars[len], str, n);
	len += n;
	return *this;
}


bool qstring::operator==(const char* str) const
{
	const char* p = chars;
	const char* stopper = &chars[len];
	while (p < stopper) {
		if (*p++ != *str++)
			return false;
		}
	return (*str == 0);
}


bool qstring::operator==(string_slice slice) const
{
	const char* p = chars;
	const char* stopper = &chars[len];
	const char* sliceP = slice.begin();
	const char* sliceStopper = slice.end();
	while (true) {
		if (p >= stopper)
			return (sliceP >= sliceStopper);
		else if (sliceP >= sliceStopper)
			return false;
		if (*p++ != *sliceP++)
			return false;
		}
	return true;
}


bool qstring::operator==(qstring& str) const
{
	if (len != str.len)
		return false;
	const char* p1 = chars;
	const char* p2 = str.chars;
	const char* stopper = &chars[len];
	while (p1 < stopper) {
		if (*p1++ != *p2++)
			return false;
		}
	return true;
}


bool qstring::startsWith(const char* str) const
{
	const char* p = chars;
	const char* stopper = &chars[len];
	while (p < stopper) {
		if (*str == 0)
			return true;
		if (*p++ != *str++)
			return false;
		}
	return (*str == 0);
}


bool qstring::startsWith(qstring& str) const
{
	if (len < str.len)
		return false;
	const char* p1 = chars;
	const char* p2 = str.chars;
	const char* stopper = &chars[str.len];
	while (p1 < stopper) {
		if (*p1++ != *p2++)
			return false;
		}
	return true;
}


const char* qstring::c_str() const
{
	if (capacity == len)
		((qstring*) this)->reserve(len + capacitySlop);
	chars[len] = 0;
	return chars;
}


char* qstring::detach_c_str()
{
	c_str();	// put the null terminator on
	char* result = chars;
	chars = new char[initialCapacity];
	capacity = initialCapacity;
	len = 0;
	return result;
}


void qstring::reserve(int newCapacity, bool retainValue)
{
	if (newCapacity < len + 1)
		return;
	capacity = newCapacity;
	char* newChars = new char[newCapacity];
	if (retainValue)
		memcpy(newChars, chars, len);
	else
		len = 0;
	delete chars;
	chars = newChars;
}


int qstring::asInt() const
{
	return atoi(c_str());
}

double qstring::asDouble() const
{
	return atof(c_str());
}



