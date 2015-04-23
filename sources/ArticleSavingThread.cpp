/* ArticleSavingThread.cpp */

#include "ArticleSavingThread.h"
#include "NNTPResponse.h"
#include "Task.h"
#include "ParseUtils.h"
#include <File.h>


ArticleSavingThread::ArticleSavingThread(const char* name, int32 priority)
	: Thread(name, priority)
{
}


status_t ArticleSavingThread::WriteFile(BFile* file, NNTPResponse* response, string_slice lineEnd, Task* task, bool forBinary)
{
	// rewind to beginning of article
	response->Reset();
	response->NextLine();	// skip result line

	// set up progress
	bool haveProgressMax = (task->GetProgressMax() != 0);
	int progressBase = task->CurProgress();

	// write
	bool inBody = false;
	int curLines = 0;
	while (!response->AtEOF()) {
		string_slice line = response->NextLine();
		if (!forBinary || (inBody && !line.empty())) {
			ssize_t result = file->Write(line.begin(), line.length());
			if (result >= 0)
				result = file->Write(lineEnd.begin(), lineEnd.length());
			if (result < 0)
				return result;
			}

		// progress
		if (!inBody) {
			if (line.length() == 0)
				inBody = true;
			else if (!haveProgressMax) {
				// check for "Lines" header
				string_slice header, value;
				ParseUtils::ParseHeader(line, &header, &value);
				if (header == "Lines") {
					task->SetProgressMax(value.asInt());
					haveProgressMax = true;
					}
				}
			}
		else {
			curLines++;
			if (task->IsReadyForUpdate())
				task->SetProgress(progressBase + curLines);
			}
		}

	task->SetProgress(progressBase + curLines);

	return B_NO_ERROR;
}


