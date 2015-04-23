/* OSException.h */

#ifndef _H_OSException_
#define _H_OSException_

#include "DisplayableException.h"
#include "qstring.h"

class OSException : public DisplayableException {
public:
	OSException(string_slice messageIn, int errCode = 0);
	OSException(string_slice message1, string_slice message2, int errCode);
	OSException(string_slice message1, string_slice message2, string_slice message3, int errCode);
	string_slice	GetMessage();
	void        	AppendOSError(int errCode);

protected:
	qstring	message;
};


#endif
