/* PostArticlesThread.cpp */

#include "PostArticlesThread.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "ConnectionGetter.h"
#include "TextReader.h"
#include "ParseUtils.h"
#include "Error.h"
#include "Autodeleter.h"
#include "Task.h"
#include "Logger.h"
#include "qstring.h"
#include <E-mail.h>
#include <Entry.h>
#include <File.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <arpa/inet.h>



PostArticlesThread::PostArticlesThread(BList* fileRefsIn)
	: OwnedThread("Post Articles"), fileRefs(fileRefsIn)
{
	/***/
}


PostArticlesThread::~PostArticlesThread()
{
	int numFileRefs = fileRefs->CountItems();
	for (int i=0; i<numFileRefs; i++)
		delete (entry_ref*) fileRefs->ItemAt(i);
	delete fileRefs;
}


void PostArticlesThread::Action()
{
	int i;

	// find out the total size (in bytes)
	int totalBytes = 0;
	int numFiles = fileRefs->CountItems();
	for (i=0; i<numFiles; i++) {
		BEntry entry((entry_ref*) fileRefs->ItemAt(i), true);
		off_t fileSize;
		if (entry.GetSize(&fileSize) == B_NO_ERROR)
			totalBytes += fileSize;
		}

	// set up progress
	Task task("Posting...");
	task.SetProgressMax(totalBytes);

	// get a connection
	ConnectionGetter connectionGetter;
	NNTPConnection* connection = connectionGetter.Connection();
	if (connection == NULL) {
		Error::ShowError("Couldn't get a connection to post articles.");
		return;
		}
	task.GotConnection();

	// post the files
	for (i=0; i<numFiles; i++)
		PostArticle((entry_ref*) fileRefs->ItemAt(i), connection, &task);
}


status_t PostArticlesThread::PostArticle(entry_ref* fileRef, NNTPConnection* connection, Task* task)
{
	status_t result = B_NO_ERROR;

	// setup progress reporting
	qstring progressString = "Posting \"";
	progressString += fileRef->name;
	progressString += "\"";
	task->SetProgressString(progressString);
	int progressBase = task->CurProgress();

	// open the file
	BFile file(fileRef, B_READ_ONLY);
	result = file.InitCheck();
	if (result != B_NO_ERROR)
		return Error::ShowError("Couldn't open \"", fileRef->name, "\".", result);

	// read the file
	off_t fileSize;
	file.GetSize(&fileSize);
	char* text = new char[fileSize];
	Autodeleter<char> textDeleter(text);
	result = file.ReadAt(0, text, fileSize);
	if (result < B_NO_ERROR)
		return Error::ShowError("Couldn't read \"", fileRef->name, "\".", result);
	TextReader reader(string_slice(text, text + fileSize));

	// log
	Logger logger(connection);

	// start sending
	connection->SendCommand("POST");
	result = connection->GetResponse();
	if (result != 340)
		return Error::ShowError("Couldn't post article.", result);

	// send the headers
	string_slice line;
	bool haveFrom = false, haveDate = false, haveReplyTo = false, haveMessageID = false;;
	while (!reader.AtEOF()) {
		line = reader.NextLine();
		if (line.length() == 0)
			break;

		// write the header
		connection->SendCommand(line);
		task->SetProgress(progressBase + reader.Position());

		// check for required headers
		string_slice headerName, headerValue;
		ParseUtils::ParseHeader(line, &headerName, &headerValue);
		if (headerName == "From")
			haveFrom = true;
		else if (headerName == "Date")
			haveDate = true;
		else if (headerName == "Reply-To" || headerName == "Reply-to")
			haveReplyTo = true;
		else if (headerName == "Message-ID")
			haveMessageID = true;
		}

	// send missing headers
	if (!haveFrom) {
		qstring* fromHeader = GetFromHeader();
		if (fromHeader)
			connection->SendCommand(*fromHeader);
		delete fromHeader;
		}
	if (!haveDate) {
		time_t curTime = time(NULL);
		char dateStr[256];
		strftime(dateStr, 256, "Date: %a, %d %b %Y %T %Z", localtime(&curTime));
		connection->SendCommand(dateStr);
		}
	if (!haveMessageID) {
		qstring* messageIDHeader = GetMessageIDHeader(connection);
		if (messageIDHeader)
			connection->SendCommand(*messageIDHeader);
		delete messageIDHeader;
		}
	if (!haveReplyTo) {
		qstring* replyToHeader = GetReplyToHeader();
		if (replyToHeader)
			connection->SendCommand(*replyToHeader);
		delete replyToHeader;
		}

	// finish header section
	connection->SendCommand("");

	// write the body
	while (!reader.AtEOF()) {
		line = reader.NextLine();

		// send the line, doubling initial period if there is one
		if (line[0] == '.') {
			qstring quotedLine = ".";
			quotedLine += line;
			connection->SendCommand(quotedLine);
			}
		else
			connection->SendCommand(line);

		task->SetProgress(progressBase + reader.Position());
		}

	// finish
	task->SetProgress(progressBase + fileSize);
	connection->SendCommand(".");
	NNTPResponse* response;
	result = connection->GetResponse(&response);
	if (result != 240) {
		qstring msg = "Couldn't post \"";
		msg += fileRef->name;
		msg += "\".  (";
		msg += response->NextLine();
		msg += ")";
		delete response;
		return Error::ShowError(msg, result);
		}
	delete response;

	return B_NO_ERROR;
}


qstring* PostArticlesThread::GetFromHeader()
{
	// ask the mail_daemon
	mail_pop_account accountInfo;
	status_t result = get_pop_account(&accountInfo);
	if (result != B_NO_ERROR)
		return NULL;

	if (accountInfo.pop_name[0] == 0 && accountInfo.reply_to[0])
		Error::ShowError("If posting fails, you should choose Preferences/E-mail from the Be menu and set up your email settings.");

	// build the "From:" header
	qstring* str = new qstring("From: ");
	if (accountInfo.real_name[0]) {
		*str += accountInfo.real_name;
		*str += " <";
		}
	if (accountInfo.reply_to[0])
		*str += accountInfo.reply_to;
	else if (accountInfo.pop_name[0]) {
		*str += accountInfo.pop_name;
		if (accountInfo.pop_host[0]) {
			*str += "@";
			*str += accountInfo.pop_host;
			}
		}
	if (accountInfo.real_name[0])
		*str += ">";
	return str;
}


qstring* PostArticlesThread::GetReplyToHeader()
{
	// ask the mail_daemon
	mail_pop_account accountInfo;
	status_t result = get_pop_account(&accountInfo);
	if (result != B_NO_ERROR)
		return NULL;
	if (accountInfo.reply_to[0] == 0)
		return NULL;

	// build the "Reply-To:" header
	qstring* str = new qstring("Reply-To: ");
	*str += accountInfo.reply_to;
	return str;
}


qstring* PostArticlesThread::GetMessageIDHeader(NNTPConnection* connection)
{
	// get the address
	struct in_addr address = connection->LocalIPAddr();

	// get the date/time info
	time_t curTime = time(NULL);
	char dateStr[256];
	strftime(dateStr, 256, "%Y%m%d%H%M%S", localtime(&curTime));
	char usecStr[128];
	sprintf(usecStr, "%06d", (int) (system_time() % 1000000));

	// build the string
	qstring* str = new qstring("Message-ID: <");
	*str += dateStr;
	*str += ".";
	*str += usecStr;
	*str += "@";
	*str += inet_ntoa(address);
	*str += ">";
	return str;
}


