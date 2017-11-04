/* ColorPrefWind.h */

#ifndef _H_ColorPrefWind_
#define _H_ColorPrefWind_

#include <Window.h>
#include <ColorControl.h>

class ColorPrefControl;

class ColorPrefWind : public BWindow {
public:
	ColorPrefWind(const char* prefNameIn, const char* title, ColorPrefControl* ownerIn,
              rgb_color defaultColor);
	~ColorPrefWind();
	void	MessageReceived(BMessage* message);

protected:
	BColorControl*        	colorControl;
	const char*           	prefName;
	ColorPrefControl*     	owner;
	static const float    	cellSide;
	static const rgb_color	bgndColor;
};


#endif
