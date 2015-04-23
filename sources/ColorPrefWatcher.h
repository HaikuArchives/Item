/* ColorPrefWatcher.h */

#ifndef _H_ColorPrefWatcher_
#define _H_ColorPrefWatcher_

#include "PrefsChangeClient.h"
#include <GraphicsDefs.h>

class ColorPrefWatcher : public PrefsChangeClient {
public:
	ColorPrefWatcher(const char* prefNameIn, rgb_color* colorIn, rgb_color defaultColor);
	void	PrefChanged(string_slice changedPrefName);

protected:
	const char*	prefName;
	rgb_color* 	color;
};


#endif
