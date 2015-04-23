/* Error.h */

#ifndef _H_Error_
#define _H_Error_

#include "string_slice.h"

class Error {
public:
	static int        	ShowError(string_slice msgIn, int err = 0);
	static int        	ShowError(string_slice msg1, string_slice msg2, int err = 0);
	static int        	ShowError(string_slice msg1, string_slice msg2, string_slice msg3, int err = 0);
	static const char*	ErrorString(int err);
};


#endif
