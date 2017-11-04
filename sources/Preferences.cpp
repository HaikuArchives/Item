/* Preferences.cpp */

#include "Preferences.h"
#include "PrefsChangeClient.h"

#include <Message.h>
#include <List.h>
#include <Path.h>
#include <File.h>
#include <Entry.h>
#include <FindDirectory.h>
#include <stdio.h>	// just for SEEK_SET

//*** test...

Preferences* Preferences::preferences = NULL;

const char* defaultListFont = "Dutch801 Rm BT\tRoman\t13";
const char* defaultExtraInfoFont = "Swis721 BT\tItalic\t9";


Preferences::Preferences(const char* prefsFileName)
	: file(NULL)
{
	prefs = new BMessage();
	changeClients = new BList();

	// open the file
	BPath path;
	initStatus = find_directory(B_USER_SETTINGS_DIRECTORY, &path, true);
	if (initStatus != B_NO_ERROR)
		return;
	path.Append(prefsFileName);
	file = new BFile(path.Path(), B_READ_WRITE | B_CREATE_FILE);
	initStatus = file->InitCheck();
	if (initStatus != B_NO_ERROR) {
		delete file;
		file = NULL;
		return;
		}

	// read the prefs
	file->Seek(0, SEEK_SET);
	prefs->Unflatten(file);
}


Preferences::~Preferences()
{
	Save();
	delete file;
	delete prefs;
	delete changeClients;
}


status_t Preferences::InitCheck()
{
	return initStatus;
}


void Preferences::Save()
{
	// sanity clause
	if (prefs == NULL || file == NULL)
		return;

	// save
	file->Seek(0, SEEK_SET);
	ssize_t bytesWritten;
	prefs->Flatten(file, &bytesWritten);
	file->SetSize(bytesWritten);
}


bool Preferences::GetBoolPref(const char* prefName, bool defaultVal)
{
	if (prefs == NULL || !prefs->HasBool(prefName))
		return defaultVal;
	return prefs->FindBool(prefName);
}


void Preferences::SetBoolPref(const char* prefName, bool value)
{
	// sanity clause
	if (prefs == NULL)
		return;

	// add or replace
	if (prefs->HasBool(prefName))
		prefs->ReplaceBool(prefName, value);
	else
		prefs->AddBool(prefName, value);
	PrefChanged(prefName);
}


int32 Preferences::GetInt32Pref(const char* prefName, int32 defaultVal)
{
	if (prefs == NULL || !prefs->HasInt32(prefName))
		return defaultVal;
	return prefs->FindInt32(prefName);
}


void Preferences::SetInt32Pref(const char* prefName, int32 value)
{
	// sanity clause
	if (prefs == NULL)
		return;

	// add or replace
	if (prefs->HasInt32(prefName))
		prefs->ReplaceInt32(prefName, value);
	else
		prefs->AddInt32(prefName, value);
	PrefChanged(prefName);
}


const char* Preferences::GetStringPref(const char* prefName, const char* defaultVal)
{
	if (prefs == NULL || !prefs->HasString(prefName))
		return defaultVal;
	return prefs->FindString(prefName);
}


void Preferences::SetStringPref(const char* prefName, const char* value)
{
	// sanity clause
	if (prefs == NULL)
		return;

	// add or replace
	if (prefs->HasString(prefName))
		prefs->ReplaceString(prefName, value);
	else
		prefs->AddString(prefName, value);
	PrefChanged(prefName);
}


BRect Preferences::GetRectPref(const char* prefName)
{
	if (prefs == NULL || !prefs->HasRect(prefName))
		return BRect();		// return invalid rect
	return prefs->FindRect(prefName);
}


void Preferences::SetRectPref(const char* prefName, BRect rect)
{
	// sanity clause
	if (prefs == NULL)
		return;

	// add or replace
	if (prefs->HasRect(prefName))
		prefs->ReplaceRect(prefName, rect);
	else
		prefs->AddRect(prefName, rect);
	PrefChanged(prefName);
}


entry_ref* Preferences::GetRefPref(const char* prefName)
{
	if (prefs == NULL || !prefs->HasRef(prefName))
		return NULL;
	entry_ref* ref = new entry_ref;
	prefs->FindRef(prefName, ref);
	return ref;
}


void Preferences::SetRefPref(const char* prefName, const entry_ref* ref)
{
	// sanity clause
	if (prefs == NULL)
		return;

	// add or replace
	if (prefs->HasRef(prefName))
		prefs->ReplaceRef(prefName, ref);
	else
		prefs->AddRef(prefName, ref);
	PrefChanged(prefName);
}


rgb_color Preferences::GetColorPref(const char* prefName, rgb_color defaultVal)
{
	if (prefs == NULL || !prefs->HasInt32(prefName))
		return defaultVal;

	// convert from int32 in RGBA order
	uint32 value = prefs->FindInt32(prefName);
	rgb_color color;
	color.red = value >> 24;
	color.green = (value >> 16) & 0x00FF;
	color.blue = (value >> 8) & 0x00FF;
	color.alpha = value & 0x00FF;
	return color;
}


void Preferences::SetColorPref(const char* prefName, rgb_color color)
{
	// sanity clause
	if (prefs == NULL)
		return;

	// convert to int32 (RGBA order)
	int32 value =
		(((uint32) color.red) << 24 | ((uint32) color.green) << 16 |
		 ((uint32) color.blue) << 8 | color.alpha);

	// add or replace
	if (prefs->HasInt32(prefName))
		prefs->ReplaceInt32(prefName, value);
	else
		prefs->AddInt32(prefName, value);
	PrefChanged(prefName);
}


Preferences* Preferences::GetPreferences(const char* prefsFileName)
{
	if (preferences == NULL)
		preferences = new Preferences(prefsFileName);
	return preferences;
}


void Preferences::DeletePreferences()
{
	delete preferences;
	preferences = NULL;
}


void Preferences::AddChangeClient(PrefsChangeClient* client)
{
	changeClients->AddItem(client);
}


void Preferences::RemoveChangeClient(PrefsChangeClient* client)
{
	changeClients->RemoveItem(client);
}


void Preferences::PrefChanged(const char* prefName)
{
	string_slice prefNameStr(prefName);
	int32 numChangeClients = changeClients->CountItems();
	for (int32 i=0; i<numChangeClients; i++)
		((PrefsChangeClient*) changeClients->ItemAt(i))->PrefChanged(prefNameStr);
}


