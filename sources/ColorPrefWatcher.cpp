/* ColorPrefWatcher.cpp */

#include "ColorPrefWatcher.h"
#include "Prefs.h"



ColorPrefWatcher::ColorPrefWatcher(const char* prefNameIn, rgb_color* colorIn, rgb_color defaultColor)
	: prefName(prefNameIn), color(colorIn)
{
	*color = Prefs()->GetColorPref(prefName, defaultColor);
}


void ColorPrefWatcher::PrefChanged(string_slice changedPrefName)
{
	if (changedPrefName == prefName)
		*color = Prefs()->GetColorPref(prefName, *color);
}


