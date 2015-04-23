/* Prefs.cpp */

#include "Prefs.h"
#include "FileNames.h"

// defaults
const char* defaultListFont = "Dutch801 Rm BT\tRoman\t13";
const char* defaultExtraInfoFont = "Swis721 BT\tItalic\t9";


Preferences* Prefs()
{
	return Preferences::GetPreferences(FileNames::prefsFileName);
}


