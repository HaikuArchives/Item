/* Logger.h */

#ifndef _H_Logger_
#define _H_Logger_

#include "string_slice.h"
#include <File.h>

class NNTPConnection;

class Logger {
public:
	Logger(NNTPConnection* connectionIn);
	~Logger();
	void	Log(string_slice line);

protected:
	BFile          	logFile;
	NNTPConnection*	connection;
	bool           	logging;
};


#endif
