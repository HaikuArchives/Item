/* ColorPrefWind.cpp */

#include "ColorPrefWind.h"
#include "ColorPrefControl.h"
#include "Prefs.h"
#include <ColorControl.h>
#include <Screen.h>

static const int32 ColorChangedMessage = 'ClrD';

const float ColorPrefWind::cellSide = 8;
const rgb_color ColorPrefWind::bgndColor = { 216, 216, 216, 255 };


ColorPrefWind::ColorPrefWind(const char* prefNameIn, const char* title, ColorPrefControl* ownerIn, rgb_color defaultColor)
	: BWindow(BRect(0, 0, 1, 1), title, B_TITLED_WINDOW, B_NOT_RESIZABLE | B_NOT_ZOOMABLE),
	  prefName(prefNameIn), owner(ownerIn)
{
	// make the colorControl
	colorControl =
		new BColorControl(BPoint(0, 0), B_CELLS_32x8, cellSide, "",
		                  new BMessage(ColorChangedMessage), true);
	colorControl->SetValue(Prefs()->GetColorPref(prefName, defaultColor));
	colorControl->SetViewColor(bgndColor);
	AddChild(colorControl);

	// resize and place window
	BRect controlFrame = colorControl->Frame();
	ResizeTo(controlFrame.Width(), controlFrame.Height());
	BRect screenRect = BScreen(this).Frame();
	MoveTo((screenRect.left + screenRect.right - controlFrame.Width()) / 2,
	       (screenRect.top + screenRect.bottom - controlFrame.Height()) / 2);

	Show();
}


ColorPrefWind::~ColorPrefWind()
{
	owner->WindowClosing(this);
}


void ColorPrefWind::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case ColorChangedMessage:
			Prefs()->SetColorPref(prefName, colorControl->ValueAsColor());
			break;

		default:
			BWindow::MessageReceived(message);
			break;
		}
}


