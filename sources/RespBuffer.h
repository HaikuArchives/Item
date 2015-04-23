/* RespBuffer.h */

#ifndef _H_RespBuffer_
#define _H_RespBuffer_

#include <SupportDefs.h>

class RespBuffer {
public:
	RespBuffer(uint32 size);
	~RespBuffer();
	uint32	Length();

	char*      	start;
	char*      	end;
	char*      	buf;
	RespBuffer*	prev;
	RespBuffer*	next;
};


#endif
