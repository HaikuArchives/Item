/* qstring.h */

#ifndef _H_qstring_
#define _H_qstring_

#include "string_slice.h"

class qstring {
public:
	qstring();
	qstring(const char* str);
	qstring(string_slice slice);
	qstring(int val);
	qstring(const qstring& otherStr);
	~qstring();

	qstring&   	operator=(const char* str);
	qstring&   	operator=(string_slice slice);
	qstring&   	operator=(const qstring& str);
	qstring&   	operator=(const qstring* str) { return *this = *str; }
	qstring&   	operator+=(char c);
	qstring&   	operator+=(const char* str);
	qstring&   	operator+=(string_slice slice);
	qstring&   	operator+=(int val);
	qstring&   	operator+=(const qstring& str);
	qstring&	operator+=(const qstring* str) { return *this += *str; }
	qstring&   	append(const char* str, int n);
	bool       	operator==(const char* str) const;
	bool       	operator==(string_slice slice) const;
	bool       	operator==(qstring& str) const;
	bool       	startsWith(const char* str) const;
	bool       	startsWith(qstring& str) const;
	const char*	c_str() const;
	char*      	detach_c_str();		// empties the qstring
	int        	length() const { return len; }
	bool       	empty() const { return (len == 0); }
	void       	reserve(int newCapacity, bool retainValue = true);
	const char*	data() const { return chars; }

	operator string_slice() const { return string_slice(chars, chars+len); }
	int   	asInt() const;
	double	asDouble() const;

private:
	char*	chars;
	int  	len, capacity;
};


#endif
