/* DisplayableException.h */

#ifndef _H_DisplayableException_
#define _H_DisplayableException_

#include "string_slice.h"

class DisplayableException {
public:
	DisplayableException();
	virtual~DisplayableException();
	virtual string_slice	GetMessage() = 0;
	virtual void        	Display();
};


#endif
