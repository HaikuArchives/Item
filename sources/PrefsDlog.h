/* PrefsDlog.h */

#ifndef _H_PrefsDlog_
#define _H_PrefsDlog_

#include <Window.h>
#include <TextControl.h>

class ColorPrefControl;
class FontPrefControl;
class BCheckBox;
class BMessage;

class PrefsDlog : public BWindow {
public:
	PrefsDlog();
	~PrefsDlog();
	void      	FrameMoved(BPoint screenPoint);
	void      	DispatchMessage(BMessage* message, BHandler* handler);
	void      	MessageReceived(BMessage* message);
	bool      	QuitRequested();
	BCheckBox*	MakeCheckbox(const char* prefName, const char* label, BRect itemRect);
	void      	LayoutItemsIn(BView* parentView);
	void      	DistributeItemsAcross(BView* parentView, float top);
	void      	ResizeBox(BView* view);
	void      	ResizeToPreferredHeight(BTextControl* control);
	void      	Save();
	void      	SaveCheckbox(BCheckBox* checkbox, const char* prefName);
	void      	CheckboxClicked(BMessage* message);

protected:
	BTabView*             	tabView;
	BTextControl*         	serverControl;
	BTextControl*         	maxConnectionsControl;
	BTextControl*         	userNameControl;
	BTextControl*         	passwordControl;
	ColorPrefControl*     	listSelectedColorControl;
	ColorPrefControl*     	listHilitedColorControl;
	ColorPrefControl*     	listBgndColorControl;
	ColorPrefControl*     	listTextColorControl;
	FontPrefControl*      	listFontControl;
	FontPrefControl*      	extraInfoFontControl;
	BCheckBox*            	expandSubjectsControl;
	BCheckBox*            	showDatesControl;
	BCheckBox*            	showLinesControl;
	BTextControl*         	binariesPathControl;
	static const float    	windWidth;
	static const float    	windHeight;
	static const float    	tabInset;
	static const float    	tabMargin;
	static const float    	boxChildTop;
	static const float    	itemVSpacing;
	static const float    	buttonWidth;
	static const float    	buttonHeight;
	static const float    	buttonSpacing;
	static const float    	buttonMargin;
	static const float    	numberFieldWidth;
	static const rgb_color	bgndColor;
};


#endif
