/* LineEndType.h */

#ifndef _H_LineEndType_
#define _H_LineEndType_

#include "string_slice.h"

enum LineEndType {
	UnknownLineEndType,
	BeLineEndType,
	NetworkLineEndType,
	MacLineEndType
};

extern const string_slice lineEndFor[];


#endif
