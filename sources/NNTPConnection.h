/* NNTPConnection.h */

#ifndef _H_NNTPConnection_
#define _H_NNTPConnection_

#include "string_slice.h"
#include "qstring.h"
#include <SupportDefs.h>

class NNTPResponse;
class RespBuffer;
class Logger;

class NNTPConnection {
public:
	NNTPConnection();
	~NNTPConnection();
	int           	InitCheck();
	status_t      	Connect();
	status_t      	SendCommand(string_slice command);
	status_t      	SendLine(string_slice command);
	status_t      	SelectGroup(string_slice group);
	status_t      	GetResponse(NNTPResponse** respLineOut = NULL);
	void          	SetGroup(string_slice group);
	string_slice  	CurrentGroup();
	struct in_addr	LocalIPAddr();
	status_t      	ReadBuffer(RespBuffer* buffer);
	void          	AttachLogger(Logger* newLogger);
	void          	DetachLogger();

protected:
	int         	socketID;
	qstring     	curGroup;
	int         	initResult;
	string_slice	lastCommand;
	Logger*     	logger;
};


#endif
