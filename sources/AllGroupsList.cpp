/* AllGroupsList.cpp */

#include "AllGroupsList.h"
#include "ListableTree.h"
#include "GroupName.h"
#include "TextReader.h"
#include "Task.h"
#include "GetGroupsListThread.h"
#include "FileNames.h"
#include <Path.h>
#include <FindDirectory.h>
#include <File.h>

struct WriteGroupContext {
	BFile*	file;
	Task* 	task;
	int   	updateProgressAfter;

	WriteGroupContext(BFile* fileIn, Task* taskIn, int updateProgressAfterIn)
		: file(fileIn), task(taskIn), updateProgressAfter(updateProgressAfterIn) {}
};



AllGroupsList::AllGroupsList()
	: fileText(NULL), dirty(false)
{
	status_t result = ReadFile();
	if (result != B_NO_ERROR)
		StartThread(new GetGroupsListThread(this));
}


AllGroupsList::~AllGroupsList()
{
	KillOwnedThreads();

	if (dirty)
		Save();

	delete fileText;
}


status_t AllGroupsList::ReadFile()
{
	status_t err;

	// figure out where the file should be
	BPath path;
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (err != B_NO_ERROR)
		return err;
	path.Append(FileNames::groupsFileName);

	// open the file
	BFile* file = new BFile(path.Path(), B_READ_ONLY);
	err = file->InitCheck();
	if (err != B_NO_ERROR) {
		delete file;
		return err;
		}
	off_t fileSize;
	file->GetSize(&fileSize);

	Task task("Reading groups...");
	task.SetProgressMax(fileSize);
	task.GotConnection();

	// load the text
	fileText = new char[fileSize];
	file->ReadAt(0, fileText, fileSize);
	delete file;

	// parse it
	TextReader reader(string_slice(fileText, fileText + fileSize));
	int updateAfter = fileReadUpdateQuantum;
	while (!reader.AtEOF()) {
		// get the groupName
		string_slice groupName = reader.NextLine();
		if (groupName.length() > 0) {
			tree->InsertObject(new GroupName(groupName));

			--updateAfter;
			if (updateAfter <= 0) {
				task.SetProgress(reader.Position());
				updateAfter = fileReadUpdateQuantum;
				}
			}
		}

	return err;
}


void AllGroupsList::Save()
{
	status_t err;

	// figure out where to put it
	BPath path;
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (err != B_NO_ERROR)
		return;
	path.Append(FileNames::groupsFileName);

	// open the file
	BFile* file = new BFile(path.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file->InitCheck() != B_NO_ERROR) {
		delete file;
		return;
		}

	Task task("Saving groups list...");
	task.SetProgressMax(tree->NumObjects());
	task.GotConnection();

	// write
	WriteGroupContext saveContext(file, &task, fileWriteUpdateQuantum);
	tree->WalkObjects(WriteGroup, &saveContext);

	// clean up
	delete file;
}


void AllGroupsList::Dirtify()
{
	dirty = true;
}


void AllGroupsList::WriteGroup(OrderedObject* object, void* data)
{
	GroupName* groupName = (GroupName*) object;
	WriteGroupContext* context = (WriteGroupContext*) data;
	BFile* file = context->file;
	static const string_slice eol("\n");

	// write the group name
	string_slice name = groupName->Name();
	file->Write(name.begin(), name.length());
	file->Write(eol.begin(), eol.length());

	// progress
	--context->updateProgressAfter;
	if (context->updateProgressAfter <= 0) {
		context->task->BumpProgress(fileWriteUpdateQuantum);
		context->updateProgressAfter = fileWriteUpdateQuantum;
		}
}


