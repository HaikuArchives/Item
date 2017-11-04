/* NNTPConnection.cpp */

#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "RespBuffer.h"
#include "Logger.h"
#include "Preferences.h"
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>



NNTPConnection::NNTPConnection()
	: logger(NULL)
{
	initResult = B_NO_INIT;
}


NNTPConnection::~NNTPConnection()
{
	if (socketID != -1)
		close(socketID);
}


int NNTPConnection::InitCheck()
{
	return initResult;
}


status_t NNTPConnection::Connect()
{
	initResult = B_NO_ERROR;

	// look up the server
	const char* serverName = Prefs()->GetStringPref("nntpServer");
	const unsigned short port = 119;		//*** make this a pref
	hostent* hostEntry = gethostbyname(serverName);
	if (hostEntry == NULL) {
		initResult = errno;
		return initResult;
		}
	uint32 ipAddress = *(uint32 *) hostEntry->h_addr;

	// open the socket & connect
	socketID = socket(AF_INET, SOCK_STREAM, 0);
	if (socketID == -1) {
		initResult = errno;
		return initResult;
		}
	sockaddr_in remoteSocket;
	remoteSocket.sin_family = AF_INET;
	remoteSocket.sin_port = htons(port);
	remoteSocket.sin_addr.s_addr = ipAddress;
	memset(remoteSocket.sin_zero, 0, 4);
	int err = connect(socketID, (sockaddr*) &remoteSocket, sizeof(remoteSocket));
	if (err != B_NO_ERROR) {
		initResult = errno;
		close(socketID);
		socketID = -1;
		return initResult;
		}

	// read connection response
	err = GetResponse();	// should be 200 or 201
	if (err != 200 && err != 201) {
		initResult = err;
		return initResult;
		}

	// authenticate if appropriate
	string_slice userName = Prefs()->GetStringPref("userName");
	if (!userName.empty()) {
		// send the userName
		qstring command = "AUTHINFO USER ";
		command += userName;
		if (logger)
			logger->Log(command);
		int result = SendLine(command);
		if (result < B_NO_ERROR) {
			initResult = result;
			return initResult;
			}

		// get the response to the userName
		NNTPResponse* response = new NNTPResponse(this);
		result = response->Read(logger);
		delete response;
		if (result < B_NO_ERROR) {
			initResult = result;
			return initResult;
			}

		// send the password if requested
		if (result == 381) {
			// send the password
			string_slice password = Prefs()->GetStringPref("password");
			command = "AUTHINFO PASS ";
			command += password;
			if (logger)
				logger->Log(command);
			result = SendLine(command);
			if (result < B_NO_ERROR) {
				initResult = result;
				return initResult;
				}

			// get the response
			response = new NNTPResponse(this);
			result = response->Read(logger);
			delete response;
			if (result < B_NO_ERROR) {
				initResult = result;
				return initResult;
				}
			}
		}

	return initResult;
}


status_t NNTPConnection::SendCommand(string_slice command)
{
	// log the command
	if (logger)
		logger->Log(command);

	// send the command
	status_t result = SendLine(command);
	if (result == ECONNRESET || result == ENOTCONN || result == EBADF || result == -1) {
		// failed--quite possibly the server timed out the connection, so we'll reconnect

		// close the socket (locally)
		qstring group = curGroup;	// save this before we clear it
		close(socketID);
		socketID = -1;
		curGroup = "";

		// reconnect
		result = Connect();

		// reselect the group
		if (result == B_NO_ERROR && group.length() > 0)
			result = SelectGroup(group);

		// resend the command
		if (result == B_NO_ERROR)
			result = SendLine(command);
		}

	lastCommand = command;
	return result;
}


status_t NNTPConnection::SendLine(string_slice command)
{
	ssize_t bytesSent;

	if (command.length() > 0) {		// net_server crashes on zero-length send
		bytesSent = send(socketID, command.begin(), command.length(), 0);
		if (bytesSent < 0)
			return errno;
		}
	bytesSent = send(socketID, "\r\n", 2, 0);
	return (bytesSent < 0 ? bytesSent : B_NO_ERROR);
}


status_t NNTPConnection::SelectGroup(string_slice group)
{
	// kick out if already there
	if (group == curGroup)
		return B_NO_ERROR;

	// send the "group" command
	qstring cmd = "group ";
	cmd += group;
	status_t result = SendCommand(cmd);
	if (result != B_NO_ERROR)
		return result;

	// check the reply
	result = GetResponse();
	if (result != 211)
		return result;

	// set & return
	curGroup = group;
	return B_NO_ERROR;
}


status_t NNTPConnection::GetResponse(NNTPResponse** respLineOut)
{
	// returns the NNTP result code, or an error code.
	// if "respLine" is given, it will be filled by an NNTPResponse.
	// blocks until the response is received.

	if (respLineOut)
		*respLineOut = NULL;	// until we determine otherwise

	NNTPResponse* response = new NNTPResponse(this);
	status_t result = response->Read(logger);
	if (result < B_NO_ERROR) {
		delete response;
		return result;
		}

	// handle authentication
	// actually, this is probably never used now that we do "preemptive" authentication
	if (result == 250) {
		delete response;
		string_slice userName = Prefs()->GetStringPref("userName");
		if (userName.empty())
			return result;

		// send the userName
		qstring command = "AUTHINFO USER ";
		command += userName;
		if (logger)
			logger->Log(command);
		result = SendLine(command);
		if (result < B_NO_ERROR)
			return result;

		// get the response to the userName
		response = new NNTPResponse(this);
		result = response->Read(logger);
		delete response;
		if (result < B_NO_ERROR)
			return result;

		// send the password if requested
		if (result == 381) {
			// send the password
			string_slice password = Prefs()->GetStringPref("password");
			command = "AUTHINFO PASS ";
			command += password;
			if (logger)
				logger->Log(command);
			result = SendLine(command);
			if (result < B_NO_ERROR)
				return result;

			// get the response
			response = new NNTPResponse(this);
			result = response->Read(logger);
			delete response;
			}

		// resend the command if authentication succeeded
		if (result == 281) {
			if (logger)
				logger->Log(lastCommand);
			result = SendLine(lastCommand);
			if (result < B_NO_ERROR)
				return result;
			response = new NNTPResponse(this);
			result = response->Read(logger);
			if (result < B_NO_ERROR) {
				delete response;
				return result;
				}
			}
		}

	// return result
	if (respLineOut)
		*respLineOut = response;
	else
		delete response;
	return result;
}


void NNTPConnection::SetGroup(string_slice group)
{
	// for use by callers that send their own "group" commands
	curGroup = group;
}


string_slice NNTPConnection::CurrentGroup()
{
	return curGroup;
}


struct in_addr NNTPConnection::LocalIPAddr()
{
	struct sockaddr_in interface;
	socklen_t size = sizeof(interface);
	getsockname(socketID, (struct sockaddr*)&interface, &size);
	return interface.sin_addr;
}


status_t NNTPConnection::ReadBuffer(RespBuffer* buffer)
{
	ssize_t bytesRead = 0;
	do {
		bytesRead = recv(socketID, buffer->start, buffer->Length(), 0);
	} while (bytesRead == 0);
	if (bytesRead < 0) {
		buffer->end = buffer->start;
		return errno;
		}
	buffer->end = buffer->start + bytesRead;
	return B_NO_ERROR;
}


void NNTPConnection::AttachLogger(Logger* newLogger)
{
	logger = newLogger;
}


void NNTPConnection::DetachLogger()
{
	logger = NULL;
}


