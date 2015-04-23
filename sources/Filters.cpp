/* Filters.cpp */

#include "Filters.h"
#include "FilterGroup.h"
#include "Filter.h"
#include "FileNames.h"
#include "TextReader.h"
#include <Path.h>
#include <FindDirectory.h>
#include <File.h>

//*** test
#include "Filter.h"

class FilterGroupSearcher : public OrderedObject {
public:
	FilterGroupSearcher(string_slice groupNameIn)
		: groupName(groupNameIn) {}

	bool	operator<(const OrderedObject& otherObject)
				{ return groupName < ((FilterGroup*) &otherObject)->GroupName(); }
	bool	operator==(const OrderedObject& otherObject)
				{ return groupName == ((FilterGroup*) &otherObject)->GroupName(); }

protected:
	string_slice	groupName;
};

const char* Filters::allGroupsName = "All Groups";


Filters::Filters()
{
	// always has "All Groups"
	AddFilterGroup(new FilterGroup(string_slice(allGroupsName)));

	Load();

//*** test
/***
FilterGroup* testGroup = new FilterGroup("rec.audio.pro");
Filter* testFilter = new Filter();
testFilter->SetString("Folta");
testGroup->AddFilter(testFilter);
testGroup->AddFilter(new Filter(Hilite, Author, "Dorsey"));
testGroup->AddFilter(new Filter(Hilite, Author, "Fletcher"));
testGroup->AddFilter(new Filter(Hilite, Author, "Gerst"));
testGroup->AddFilter(new Filter(Hilite, Author, "McQuilken"));
testGroup->AddFilter(new Filter(Kill, Subject, "Spam Pan"));
AddFilterGroup(testGroup);
GetFilterGroup(string_slice(allGroupsName))->AddFilter(new Filter(Hilite, Author, "Folta"));
***/
}


Filters::~Filters()
{
	Save();
}


FilterGroup* Filters::GetFilterGroup(string_slice groupName)
{
	FilterGroupSearcher searcher(groupName);
	return (FilterGroup*) FindObject(&searcher);
}


FilterGroup* Filters::GetGlobalFilters()
{
	return GetFilterGroup(string_slice(allGroupsName));
}


void Filters::AddFilterGroup(FilterGroup* filterGroup)
{
	InsertObject(filterGroup);
}


void Filters::Save()
{
	status_t err;

	// figure out where to put it
	BPath path;
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (err != B_NO_ERROR)
		return;
	path.Append(FileNames::filtersFileName);

	// open the file
	BFile* file = new BFile(path.Path(), B_READ_WRITE | B_CREATE_FILE | B_ERASE_FILE);
	if (file->InitCheck() != B_NO_ERROR)
		return;

	// write
	WalkObjects(WriteFilterGroup, file);

	// clean up
	delete file;
}


void Filters::Load()
{
	status_t err;

	// figure out where the file should be
	BPath path;
	err = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (err != B_NO_ERROR)
		return;
	path.Append(FileNames::filtersFileName);

	// open the file
	BFile* file = new BFile(path.Path(), B_READ_ONLY);
	if (file->InitCheck() != B_NO_ERROR) {
		delete file;
		return;
		}

	// load the text
	off_t fileSize;
	file->GetSize(&fileSize);
	char* text = new char[fileSize];
	file->ReadAt(0, text, fileSize);
	delete file;

	// parse it
	TextReader reader(string_slice(text, text + fileSize));
	FilterGroup* curGroup = NULL;
	while (!reader.AtEOF()) {
		// get the groupName
		string_slice groupName = reader.NextTabField();
		if (groupName.length() > 0) {
			curGroup = GetFilterGroup(groupName);
			if (curGroup == NULL) {
				curGroup = new FilterGroup(groupName);
				AddFilterGroup(curGroup);
				}
			reader.NextLine();
			}

		// if none, this is a filter line (if it's not blank)
		else {
			string_slice filterLine = reader.NextLine();
			if (filterLine.length() > 0 && curGroup != NULL) {
				Filter* filter = new Filter();
				TextReader lineReader(filterLine);
				filter->ReadFrom(&lineReader);
				curGroup->AddFilter(filter);
				}
			}
		}

	// clean up
	delete text;
}


void Filters::WriteFilterGroup(OrderedObject* object, void* data)
{
	FilterGroup* filterGroup = (FilterGroup*) object;
	BFile* file = (BFile*) data;
	string_slice eol("\n");

	// write the group name
	string_slice groupName = filterGroup->GroupName();
	file->Write(groupName.begin(), groupName.length());
	file->Write(eol.begin(), eol.length());

	// write the filters
	int numFilters = filterGroup->NumFilters();
	for (int i=0; i<numFilters; i++)
		filterGroup->FilterAt(i)->WriteToFile(file);
}


