/* string_slice.h */

#ifndef _H_string_slice_
#define _H_string_slice_

#include <string.h>

class string_slice {
protected:
	const char*	start;
	const char*	stopper;

	static const unsigned int npos = (unsigned int) -1;

public:
	string_slice(const char* startIn, unsigned int len)
		: start(startIn), stopper(start + len) {}
	string_slice(const char* startIn, const char* end)
		: start(startIn), stopper(end) {}
	string_slice(const char* str)
		: start(str), stopper(start + strlen(str)) {}
	string_slice()
		: start(NULL), stopper(NULL) {}

	unsigned int 	length() const { return stopper - start; }
	bool         	empty() const { return start >= stopper; }
	char         	operator[](int index) { return start[index]; }
	string_slice&	operator=(const char* str)
						{ start = str; stopper = start + strlen(str); return *this; }

	bool        	operator==(const char* str) const;
	bool        	operator!=(const char* str) const { return !(*this == str); }
	bool        	operator==(string_slice other) const;
	bool        	operator!=(string_slice other) const { return !(*this == other); }
	bool        	operator<(string_slice other) const;
	string_slice	substr(unsigned int firstChar = 0, unsigned int numChars = npos) const;
	void        	eraseFirst(unsigned int numChars);
	void        	resize(unsigned int newSize);
	void        	copy(char* dest, unsigned int numChars = npos, unsigned int firstChar = 0) const;
	int         	asInt() const;
	double      	asDouble() const;
	bool        	containsCI(string_slice other) const;
	bool        	startsWith(string_slice other) const;
	bool        	endsWith(string_slice other) const;
	string_slice	trim() const;
	string_slice	separate(string_slice separator, string_slice* remainder = NULL) const;

	typedef const char* const_iterator;
	const_iterator	begin() const { return start; }
	const_iterator	end() const { return stopper; }
};


#endif
