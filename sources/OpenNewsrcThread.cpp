/* OpenNewsrcThread.cpp */

#include "OpenNewsrcThread.h"
#include "Newsrc.h"
#include "NewsrcWind.h"
#include "Newsgroup.h"
#include "ConnectionPool.h"
#include "NNTPConnection.h"
#include "NNTPResponse.h"
#include "Error.h"
#include <File.h>



OpenNewsrcThread::OpenNewsrcThread(entry_ref* fileRefIn)
	: Thread("OpenNewsrc"), fileRef(*fileRefIn)
{
}


void OpenNewsrcThread::Action()
{
	// open the file
	BFile* file = new BFile(&fileRef, B_READ_WRITE);
	status_t err = file->InitCheck();
	if (err != B_NO_ERROR) {
		Error::ShowError("Couldn't open \"", fileRef.name, "\".", err);
		delete file;
		return;
		}

	// create the newsrc
	Newsrc* newsrc = new Newsrc(file);

	// read the Newsrc
	newsrc->Read();

	// create the window
	new NewsrcWind(newsrc, fileRef.name);
}


