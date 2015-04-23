/* NNTPResponse.h */

#ifndef _H_NNTPResponse_
#define _H_NNTPResponse_

#include "string_slice.h"
#include <Locker.h>

class RespBuffer;
class NNTPConnection;
class NNTPResponseThreadParams;
class Logger;

class NNTPResponse : BLocker {
public:
	NNTPResponse(NNTPConnection* connectionIn);
	~NNTPResponse();
	status_t    	Read(Logger* logger = NULL);
	bool        	AtEOF();
	string_slice	NextLine();
	string_slice	NextField();
	int         	NextIntField();
	string_slice	NextTabField();
	int         	NextIntTabField();
	void        	Reset();
	void        	ReleaseThru(const char* byte);

protected:
	void        	ReadFurtherBuffers(RespBuffer* nextBuffer);

public:
	bool        	IsFinalBuffer(RespBuffer* buffer);
	bool        	SetupRead();
	void        	SpillBuffer(RespBuffer* buffer, RespBuffer* nextBuffer);
	static int32	ThreadStarter(void* paramsIn);
	int         	sstoi(string_slice ss);

protected:
	bool               	multiline;
	NNTPConnection*    	connection;
	RespBuffer*        	firstBuffer;
	RespBuffer*        	lastBuffer;
	RespBuffer*        	readBuffer;
	const char*        	readByte;
	thread_id          	readerThread;
	static const uint32	MaxStatusRespSize;
	static const uint32	LargeBufferSize;
};


#endif
