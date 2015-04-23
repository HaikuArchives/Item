/* TextReader.h */

#ifndef _H_TextReader_
#define _H_TextReader_

#include "string_slice.h"

class TextReader {
public:
	TextReader(string_slice text);
	bool        	AtEOF();
	string_slice	NextLine();
	string_slice	NextTabField();
	int         	Position();

protected:
	const char*	p;
	const char*	stopper;
	const char*	start;
};


#endif
