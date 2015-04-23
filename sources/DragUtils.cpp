/* DragUtils.cpp */

#include "DragUtils.h"
#include "TextReader.h"
#include "string_slice.h"
#include <Message.h>
#include <List.h>


BList* DragUtils::GetDraggedGroups(const BMessage* message)
	// returns a BList of string_slices
{
	BList* groups = new BList();

	// get the text
	const char* text;
	ssize_t textSize;
	status_t result = message->FindData("text/plain", B_MIME_TYPE,
	                                    (const void **) &text, &textSize);
	if (result != B_NO_ERROR)
		return groups;

	// parse it
	TextReader reader(string_slice(text, text + textSize));
	while (!reader.AtEOF()) {
		string_slice line = reader.NextLine();
		if (line.length() == 0)
			continue;
		groups->AddItem(new string_slice(line));
		}

	return groups;
}


bool DragUtils::HasNewsgroups(const BMessage* message)
{
	// get the text
	const char* text;
	ssize_t textSize;
	status_t result = message->FindData("text/plain", B_MIME_TYPE,
	                                    (const void**) &text, &textSize);
	if (result != B_NO_ERROR)
		return false;

	// parse it
	TextReader reader(string_slice(text, text + textSize));
	while (!reader.AtEOF()) {
		string_slice line = reader.NextLine();
		if (line.length() == 0)
			continue;
		if (!IsGroupName(line))
			return false;
		}

	// it passed all the tests
	return true;
}


bool DragUtils::IsGroupName(string_slice name)
{
	const char* p = name.begin();
	const char* stopper = name.end();
	for (; p<stopper; p++) {
		if (*p <= ' ' || *p > '~')
			return false;
		}
	return true;
}


