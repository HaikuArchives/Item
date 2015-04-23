/* SaveArticleThread.cpp */

#include "SaveArticleThread.h"
#include "ConnectionGetter.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "ParseUtils.h"
#include "Task.h"
#include "Error.h"
#include <File.h>
#include <Locker.h>
#include <Entry.h>
#include <stdio.h>

const string_slice SaveArticleThread::divider = "----------------------------------------------------------------------";


SaveArticleThread::SaveArticleThread(copies BMessage* messageIn)
	: ArticleSavingThread("Save Article"), message(*messageIn)
{
}


void SaveArticleThread::Action()
{
	// get the info out of the message
	entry_ref fileRef;
	status_t result = message.FindRef("refs", &fileRef);
	if (result != B_NO_ERROR)
		return;
	string_slice groupName(message.FindString("groupName"));
	if (groupName.length() == 0)
		return;

	// save the messages
	for (int i=0; ; i++) {
		// get the info
		int32 articleNo = message.FindInt32("articleNos", i);
		if (articleNo == 0)
			break;
		string_slice subject(message.FindString("subjects", i));
		if (subject.length() == 0)
			break;

		// save
		SaveMessage(&fileRef, groupName, articleNo, subject);
		}
}


void SaveArticleThread::SaveMessage(entry_ref* fileRef, string_slice groupName, int articleNo, string_slice subject)
{
	status_t result;

	// set up progress
	qstring progressStr = subject;
	progressStr += " -> ";
	progressStr += fileRef->name;
	Task task(progressStr);

	// open the file
	BFile file(fileRef, B_READ_WRITE | B_CREATE_FILE);
	if (file.InitCheck() != B_NO_ERROR) {
		Error::ShowError("Couldn't open ", fileRef->name, ".", file.InitCheck());
		return;
		}

	// find out the line-end type
	LineEndType lineEndType = GetLineEndType(&file);
	if (lineEndType == UnknownLineEndType)
		lineEndType = BeLineEndType;	// for new file, make it a Be text file

	// get a connection
	ConnectionGetter connectionGetter;
	NNTPConnection* connection = connectionGetter.Connection();
	connection->SelectGroup(groupName);
	task.GotConnection();

	// send the command to read the article
	qstring cmd = "article ";
	cmd += articleNo;
	NNTPResponse* response = NULL;
	result = connection->SendCommand(cmd);
	if (result == B_NO_ERROR)
		result = connection->GetResponse(&response);
	if (result != 220) {
		Error::ShowError("Couldn't get article \"", subject, "\".", result);
		delete response;
		return;
		}

	// start writing the file
	string_slice lineEnd = lineEndFor[lineEndType];
	off_t fileLen;
	file.GetSize(&fileLen);
	file.Seek(fileLen, SEEK_SET);
	if (fileLen > 0) {
		// write the divider
		file.Write(lineEnd.begin(), lineEnd.length());
		file.Write(divider.begin(), divider.length());
		file.Write(lineEnd.begin(), lineEnd.length());
		file.Write(lineEnd.begin(), lineEnd.length());
		}

	// write the article into the file
	result = WriteFile(&file, response, lineEnd, &task);
	delete response;
	if (result < B_NO_ERROR) {
		Error::ShowError("Error while writing to \"", fileRef->name, "\".", result);
		return;
		}
}


LineEndType SaveArticleThread::GetLineEndType(BFile* file)
{
	LineEndType lineEndType = UnknownLineEndType;
	file->Seek(0, SEEK_SET);
	static size_t bufSize = 1024;
	char* buf = new char[bufSize];
	while (true) {
		// read a bufferfull
		ssize_t bytesRead = file->Read(buf, bufSize);
		if (bytesRead < 0)
			break;

		// find a line end
		const char* p = buf;
		const char* stopper = buf + bufSize;
		bool foundLineEnd = false;
		for (; p<stopper; p++) {
			if (*p == '\n') {
				lineEndType = BeLineEndType;
				foundLineEnd = true;
				break;
				}
			else if (*p == '\r') {
				if (p<stopper-1 && p[1] == '\n')	//** slight bug: fails if "\r\n" straddles buffer boundary
					lineEndType = NetworkLineEndType;
				else
					lineEndType = MacLineEndType;
				foundLineEnd = true;
				break;
				}
			}
		if (foundLineEnd)
			break;

		// quit if it was the last buffer
		if ((size_t) bytesRead < bufSize)
			break;
		}

	delete buf;

	return lineEndType;
}


