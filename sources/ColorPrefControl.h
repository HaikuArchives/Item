/* ColorPrefControl.h */

#ifndef _H_ColorPrefControl_
#define _H_ColorPrefControl_

#include <View.h>
#include "PrefsChangeClient.h"

class ColorPrefWind;

class ColorPrefControl : public BView, public PrefsChangeClient {
public:
	ColorPrefControl(BRect frame, const char* prefNameIn, const char* labelIn,
                 rgb_color defaultColor);
	~ColorPrefControl();
	void	Save();
	void	WindowClosing(BWindow* closingWind);
	void	Draw(BRect updateRect);
	void	GetPreferredSize(float* width, float* height);
	void	MessageReceived(BMessage* message);
	void	MouseMoved(BPoint point, uint32 transit, const BMessage* message);
	void	MouseDown(BPoint point);
	void	PrefChanged(string_slice changedPrefName);
	void	CalcBox(BRect* boxRectOut, BRegion* boxFrameRegionOut);

protected:
	const char*           	prefName;
	const char*           	label;
	rgb_color             	savedColor;
	ColorPrefWind*        	wind;
	static const float    	boxSize;
	static const float    	boxFrameSize;
	static const float    	divider;
	static const rgb_color	textColor;
	static const rgb_color	boxFrameColor;
};


#endif
