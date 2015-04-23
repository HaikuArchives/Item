/* Logger.cpp */

#include "Logger.h"
#include "NNTPConnection.h"
#include "FileNames.h"
#include <Path.h>
#include <FindDirectory.h>



Logger::Logger(NNTPConnection* connectionIn)
	: connection(connectionIn), logging(false)
{
	status_t err;

	// figure out where the file should be
	BPath path;
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (err != B_NO_ERROR)
		return;
	path.Append(FileNames::logFileName);

	// open the file
	err = logFile.SetTo(path.Path(), B_WRITE_ONLY | B_OPEN_AT_END);
	if (err != B_NO_ERROR)
		return;

	//  start logging
	logging = true;
	connection->AttachLogger(this);
}


Logger::~Logger()
{
	if (logging) {
		Log("");
		Log("");
		connection->DetachLogger();
		}
}


void Logger::Log(string_slice line)
{
	static const string_slice lineEnd = "\n";

	if (!logging)
		return;

	logFile.Write(line.begin(), line.length());
	logFile.Write(lineEnd.begin(), lineEnd.length());
}


