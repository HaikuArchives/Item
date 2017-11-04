/* Preferences.h */

#ifndef _H_Preferences_
#define _H_Preferences_

#include <SupportDefs.h>
#include <GraphicsDefs.h>
#include <Rect.h>
#include <Entry.h>

#include "FileNames.h"

class BMessage;
class BFile;
class BList;
class PrefsChangeClient;

class Preferences {
public:
	Preferences(const char* prefsFileName);
	~Preferences();
	status_t           	InitCheck();
	void               	Save();
	bool               	GetBoolPref(const char* prefName, bool defaultVal = false);
	void               	SetBoolPref(const char* prefName, bool value);
	int32              	GetInt32Pref(const char* prefName, int32 defaultVal = 0);
	void               	SetInt32Pref(const char* prefName, int32 value);
	const char*        	GetStringPref(const char* prefName, const char* defaultVal = "");
	void               	SetStringPref(const char* prefName, const char* value);
	BRect              	GetRectPref(const char* prefName);
	void               	SetRectPref(const char* prefName, BRect rect);
	entry_ref*         	GetRefPref(const char* prefName);
	void               	SetRefPref(const char* prefName, const entry_ref* ref);
	rgb_color          	GetColorPref(const char* prefName, rgb_color defaultVal);
	void               	SetColorPref(const char* prefName, rgb_color color);
	static Preferences*	GetPreferences(const char* prefsFileName);
	static void        	DeletePreferences();
	void               	AddChangeClient(PrefsChangeClient* client);
	void               	RemoveChangeClient(PrefsChangeClient* client);
	void               	PrefChanged(const char* prefName);

protected:
	BMessage*          	prefs;
	BFile*             	file;
	BList*             	changeClients;
	status_t           	initStatus;
	static Preferences*	preferences;
};

extern const char* defaultListFont;
extern const char* defaultExtraInfoFont;

inline Preferences* Prefs()
{
	return Preferences::GetPreferences(FileNames::prefsFileName);
}


#endif
