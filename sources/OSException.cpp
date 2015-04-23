/* OSException.cpp */

#include "OSException.h"
#include "Error.h"



OSException::OSException(string_slice messageIn, int errCode)
	: message(messageIn)
{
	AppendOSError(errCode);
}


OSException::OSException(string_slice message1, string_slice message2, int errCode)
	: message(message1)
{
	message += message2;
	AppendOSError(errCode);
}


OSException::OSException(string_slice message1, string_slice message2, string_slice message3, int errCode)
	: message(message1)
{
	message += message2;
	message += message3;
	AppendOSError(errCode);
}


string_slice OSException::GetMessage()
{
	return message;
}


void OSException::AppendOSError(int errCode)
{
	if (errCode != 0) {
		message += "  (";
		const char* errStr = Error::ErrorString(errCode);
		if (errStr != NULL)
			message += errStr;
		else
			message += errCode;
		message += ")";
		}
}


