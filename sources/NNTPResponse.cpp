/* NNTPResponse.cpp */

#include "NNTPResponse.h"
#include "RespBuffer.h"
#include "NNTPConnection.h"
#include "Logger.h"
#include <Autolock.h>
#include <stdlib.h>

struct NNTPResponseThreadParams {
	NNTPResponse*	response;
	RespBuffer*  	nextBuffer;

	NNTPResponseThreadParams(NNTPResponse* respIn, RespBuffer* nextBufIn)
		: response(respIn), nextBuffer(nextBufIn) {}
};

const uint32 NNTPResponse::MaxStatusRespSize = 512;
const uint32 NNTPResponse::LargeBufferSize = 2048;


NNTPResponse::NNTPResponse(NNTPConnection* connectionIn)
	: multiline(false), connection(connectionIn),
	  firstBuffer(NULL), lastBuffer(NULL), readBuffer(NULL), readByte(NULL),
	  readerThread(-1)
{
}


NNTPResponse::~NNTPResponse()
{
	if (readerThread >= 0)
		kill_thread(readerThread);
	ReleaseThru(lastBuffer->end - 1);
}


status_t NNTPResponse::Read(Logger* logger)
{
	BAutolock(this);

	// set up & read the first buffer
	firstBuffer = new RespBuffer(multiline ? LargeBufferSize : MaxStatusRespSize);
	lastBuffer = firstBuffer;
	status_t result = connection->ReadBuffer(firstBuffer);
	if (result != B_NO_ERROR)
		return result;

	// log the response
	if (logger) {
		logger->Log(NextLine());
		Reset();
		}

	// get the result code and set "multiline" based on it
	// we need this before we can call IsFinalBuffer()
	result = NextIntField();
	switch (result) {
		case 100:	case 215:	case 218:	case 220:	case 221:	case 222:	case 224:
		case 230:	case 231:	case 282:	case 288:
			multiline = true;
			break;

		default:
			multiline = false;
			break;
		}

	// if that's all, we're done
	if (IsFinalBuffer(firstBuffer))
		return result;

	// otherwise, start reading the rest of the buffers
	RespBuffer* nextBuffer = new RespBuffer(LargeBufferSize);
	SpillBuffer(firstBuffer, nextBuffer);
	NNTPResponseThreadParams* threadParams = new NNTPResponseThreadParams(this, nextBuffer);
	readerThread =
		spawn_thread(ThreadStarter, "NNTPResponse reader", B_NORMAL_PRIORITY, threadParams);
	if (readerThread < 0) {
		delete threadParams;
		return readerThread;
		}
	resume_thread(readerThread);

	return result;
}


bool NNTPResponse::AtEOF()
{
	BAutolock(this);
	return (readBuffer == lastBuffer && readByte >= lastBuffer->end && readerThread < 0);
}


string_slice NNTPResponse::NextLine()
{
	// NOTE: will return an extra blank line at the end

	if (!SetupRead())
		return string_slice();

	// find EOL
	const char* lineStart = readByte;
	const char* p = readByte;
	const char* stopper = readBuffer->end - 1;
	for (; p < stopper; p++) {
		if (*p == '\r' && p[1] == '\n')
			break;
		}
	readByte = p + 2;	// skip the "\r\n"

	// un-escape lines beginning with '.'
	if (*lineStart == '.')
		lineStart++;

	return string_slice(lineStart, p);
}


string_slice NNTPResponse::NextField()
{
	if (!SetupRead())
		return string_slice();

	// find the field
	const char* p = readByte;
	const char* stopper = readBuffer->end;
	// skip initial whitespace
	for (; p < stopper; p++) {
		char c = *p;
		if (c != ' ' && c != '\t' && c != '\r' && c != '\n')
			break;
		}
	// read the field, up until the next whitespace
	const char* fieldStart = p;
	for (; p < stopper; p++) {
		char c = *p;
		if (c == ' ' || c == '\t' || c == '\r' || c == '\n')
			break;
		}
	readByte = p;

	return string_slice(fieldStart, p);
}


int NNTPResponse::NextIntField()
{
	return sstoi(NextField());
/***
	string_slice field = NextField();

	// convert to int
	char fieldStr[256];
	if (field.length() > 255)
		field.resize(255);
	int len = field.length();
	memcpy(fieldStr, field.begin(), len);
	fieldStr[len] = 0;
	return atoi(fieldStr);
***/
}


string_slice NNTPResponse::NextTabField()
{
	if (!SetupRead())
		return string_slice();

	// find the field
	const char* p = readByte;
	const char* stopper = readBuffer->end;
	const char* fieldStart = p;
	for (; p < stopper; p++) {
		char c = *p;
		if (c == '\t' || c == '\r' || c == '\n')
			break;
		}
	string_slice result(fieldStart, p);

	// skip the tab
	if (p < stopper && *p == '\t')
		p++;
	readByte = p;

	return result;
}


int NNTPResponse::NextIntTabField()
{
	return sstoi(NextTabField());
}


void NNTPResponse::Reset()
{
	readBuffer = NULL;
	SetupRead();
}


void NNTPResponse::ReleaseThru(const char* byte)
{
	// release all buffers up to the one containing "byte" (and that one too if
	// "byte" is the last byte of that buffer.

	Lock();
	while (firstBuffer) {
		RespBuffer* buffer = firstBuffer;

		// if this is the buffer containing "byte", stop
		if (byte > buffer->buf && byte < buffer->end) {
			if (byte == buffer->end - 1) {
				// "byte" is the last byte in the buffer; delete this buffer too
				firstBuffer = buffer->next;
				delete buffer;
				}
			break;
			}

		// delete this buffer
		firstBuffer = buffer->next;
		delete buffer;
		}
	// finish up deletions
	if (firstBuffer)
		firstBuffer->prev = NULL;
	else
		lastBuffer = NULL;
	Unlock();
}


void NNTPResponse::ReadFurtherBuffers(RespBuffer* nextBuffer)
{
	// this is the body of the spawned thread that reads all buffers after the first

	while (true) {
		// read the new buffer
		RespBuffer* newBuffer = nextBuffer;
		connection->ReadBuffer(newBuffer);
		newBuffer->start = newBuffer->buf;	// set start back to beginning; the spill may have
		                                  	// set it into the buffer

		// see if it's the last one
		bool isFinalBuffer = IsFinalBuffer(newBuffer);

		// spill if necessary
		if (!isFinalBuffer) {
			nextBuffer = new RespBuffer(LargeBufferSize);
			SpillBuffer(newBuffer, nextBuffer);
			}

		// link it in
		Lock();
		if (lastBuffer)
			lastBuffer->next = newBuffer;
		newBuffer->prev = lastBuffer;
		lastBuffer = newBuffer;
		if (firstBuffer == NULL)
			firstBuffer = newBuffer;
		Unlock();

		if (isFinalBuffer)
			break;
		}

	// done; mark as such
	readerThread = -1;
}


bool NNTPResponse::IsFinalBuffer(RespBuffer* buffer)
{
	if (multiline)
		if (buffer->end - buffer->start >= 5)
			return (string_slice(buffer->end - 5, buffer->end) == "\r\n.\r\n");
		else
			return (string_slice(buffer->end - 3, buffer->end) == ".\r\n");
	else
		return (string_slice(buffer->end - 2, buffer->end) == "\r\n");
}


bool NNTPResponse::SetupRead()
{
	// if we haven't started reading, set up
	if (readBuffer == NULL) {
		if (firstBuffer == NULL)
			return false;
		readBuffer = firstBuffer;
		readByte = readBuffer->start;
		}

	// if we're done with the current buffer, go to next buffer
	while (readByte >= readBuffer->end) {
		if (readBuffer->next) {
			readBuffer = readBuffer->next;
			readByte = readBuffer->start;
			}
		else if (readBuffer == lastBuffer && readerThread < 0) {
			// at EOF
			return false;
			}
		else {
			// we're read all the buffers, but we're still waiting for one to come in
			// wait for it...
			while (true) {
				snooze(20000);
				Lock();
				bool haveNewBuffer = (lastBuffer != readBuffer);
				Unlock();
				if (haveNewBuffer)
					break;
				}
			// go to it
			readBuffer = readBuffer->next;
			readByte = readBuffer->start;
			}
		}

	return true;
}


void NNTPResponse::SpillBuffer(RespBuffer* buffer, RespBuffer* nextBuffer)
{
	// find last EOL
	char* stopper = buffer->start + 1;
	char* p = buffer->end - 1;
	for (; p >= stopper; --p) {
		if (*p == '\n' && p[-1] == '\r')
			break;
		}
	if (p == buffer->end - 1)
		return;		// ends at line boundary, don't spill
	if (p >= stopper)
		p++;	// skip the '\n'

	// spill to nextBuffer
	uint32 spillLength = buffer->end - p;
	memcpy(nextBuffer->buf, p, spillLength);
	buffer->end = p;
	nextBuffer->start += spillLength;
}


int32 NNTPResponse::ThreadStarter(void* paramsIn)
{
	NNTPResponseThreadParams* paramsObj = (NNTPResponseThreadParams*) paramsIn;
	NNTPResponseThreadParams params = *paramsObj;
	delete paramsObj;

	params.response->ReadFurtherBuffers(params.nextBuffer);

	return B_NO_ERROR;
}


int NNTPResponse::sstoi(string_slice ss)
{
	char str[256];
	if (ss.length() > 255)
		ss.resize(255);
	int len = ss.length();
	memcpy(str, ss.begin(), len);
	str[len] = 0;
	return atoi(str);
}


