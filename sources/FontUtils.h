/* FontUtils.h */

#ifndef _H_FontUtils_
#define _H_FontUtils_

#include "qstring.h"

class BFont;

class FontUtils {
public:
	static void	FontToString(const BFont* font, qstring* str);
	static void	StringToFont(const string_slice str, BFont* font, bool* isSystemFont = NULL);
};


#endif
