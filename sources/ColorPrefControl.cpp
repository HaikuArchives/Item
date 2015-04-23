/* ColorPrefControl.cpp */

#include "ColorPrefControl.h"
#include "Prefs.h"
#include "ColorPrefWind.h"
#include <Region.h>
#include <Message.h>

const float ColorPrefControl::boxSize = 18;
const float ColorPrefControl::boxFrameSize = 2;
const float ColorPrefControl::divider = 4;
const rgb_color ColorPrefControl::textColor = { 0, 0, 0, 255 };
const rgb_color ColorPrefControl::boxFrameColor = { 255, 255, 255, 255 };


ColorPrefControl::ColorPrefControl(BRect frame, const char* prefNameIn, const char* labelIn, rgb_color defaultColor)
	: BView(frame, prefNameIn, B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP,
	        B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE),
	  prefName(prefNameIn), label(labelIn), wind(NULL)
{
	savedColor = Prefs()->GetColorPref(prefName, defaultColor);
}


ColorPrefControl::~ColorPrefControl()
{
	if (wind)
		wind->Close();

	// revert to the saved color
	Prefs()->SetColorPref(prefName, savedColor);
}


void ColorPrefControl::Save()
{
	savedColor = Prefs()->GetColorPref(prefName, savedColor);
}


void ColorPrefControl::WindowClosing(BWindow* closingWind)
{
	if (closingWind == wind)
		wind = NULL;
}


void ColorPrefControl::Draw(BRect updateRect)
{
	SetLowColor(ViewColor());
	BRect bounds = Bounds();

	// draw the label
	SetHighColor(textColor);
	font_height fontInfo;
	GetFontHeight(&fontInfo);
	BPoint labelStart(0, floor((bounds.top + bounds.bottom + fontInfo.ascent) / 2));
	DrawString(label, labelStart);

	// draw the box
	BRect box;
	BRegion frameRgn;
	CalcBox(&box, &frameRgn);
	// frame
	SetHighColor(boxFrameColor);
/***
	BRect frameRect = box;
	frameRect.InsetBy(-boxFrameSize, -boxFrameSize);
	frameRgn.Set(frameRect);
	frameRgn.Exclude(box);
***/
	FillRegion(&frameRgn);
/***
	SetPenSize(boxFrameSize);
	SetHighColor(boxFrameColor);
	StrokeRect(boxFrame);
***/
	// box
	SetHighColor(Prefs()->GetColorPref(prefName, savedColor));
	FillRect(box);
}


void ColorPrefControl::GetPreferredSize(float* width, float* height)
{
	*width = StringWidth(label) + divider + boxSize + 2 * boxFrameSize;
	*height = boxSize + boxFrameSize * 2;
}


void ColorPrefControl::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case B_PASTE:
			{
			const rgb_color* color;
			ssize_t numBytes = sizeof(color);
			status_t result =
				message->FindData("RGBColor", B_RGB_COLOR_TYPE, (const void**) &color, &numBytes);
			if (result == B_NO_ERROR) {
				Prefs()->SetColorPref(prefName, *color);
				}
			}
			break;

		default:
			BView::MessageReceived(message);
			break;
		}
}


void ColorPrefControl::MouseMoved(BPoint point, uint32 transit, const BMessage* message)
{
	// check for drag
	if (message != NULL && (transit == B_ENTERED_VIEW || transit == B_EXITED_VIEW)) {
		if (message->what == B_PASTE && message->HasData("RGBColor", B_RGB_COLOR_TYPE)) {
			BRegion boxFrameRgn;
			CalcBox(NULL, &boxFrameRgn);
			SetHighColor(transit == B_ENTERED_VIEW ? keyboard_navigation_color() : boxFrameColor);
			FillRegion(&boxFrameRgn);
			}
		}
}


void ColorPrefControl::MouseDown(BPoint point)
{
	if (wind)
		wind->Activate();
	else
		wind = new ColorPrefWind(prefName, label, this, savedColor);
}


void ColorPrefControl::PrefChanged(string_slice changedPrefName)
{
	if (changedPrefName == prefName) {
		BWindow* parentWind = Window();
		if (parentWind->Lock()) {
			Invalidate();
			parentWind->Unlock();
			}
		}
}


void ColorPrefControl::CalcBox(BRect* boxRectOut, BRegion* boxFrameRegionOut)
{
	BRect bounds = Bounds();

	// box
	BRect box;
	box.top = floor((bounds.top + bounds.bottom - boxSize) / 2);
	box.bottom = box.top + boxSize - 1;
	box.right = bounds.right - boxFrameSize;
	box.left = box.right - boxSize + 1;
	if (boxRectOut)
		*boxRectOut = box;

	// frame
	if (boxFrameRegionOut) {
		BRect frameRect = box;
		frameRect.InsetBy(-boxFrameSize, -boxFrameSize);
		boxFrameRegionOut->Set(frameRect);
		boxFrameRegionOut->Exclude(box);
		}
}


