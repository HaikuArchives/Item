/* ParseUtils.h */

#ifndef _H_ParseUtils_
#define _H_ParseUtils_

#include "string_slice.h"

class ParseUtils {
public:
	static void	ParseHeader(string_slice line, string_slice* name, string_slice* value);
};


#endif
