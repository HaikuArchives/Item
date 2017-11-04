/* FontPrefControl.h */

#ifndef _H_FontPrefControl_
#define _H_FontPrefControl_

#include <View.h>
#include <Window.h>
#include <TextControl.h>

#include "string_slice.h"
#include "qstring.h"

class FontPrefControl : public BView {
public:
	FontPrefControl(const char* prefNameIn, string_slice labelIn, BRect frame, uint32 resizingMode);
	~FontPrefControl();
	void	Save();
	void	SetPref();
	void	Draw(BRect updateRect);
	void	GetPreferredSize(float* width, float* height);
	void	MouseDown(BPoint point);
	void	MessageReceived(BMessage* message);
	void	PopStyleMenu();
	void	PopFamilyMenu();
	void	StartSizeEdit();
	void	FinishSizeEdit();

protected:
	const char*       	prefName;
	string_slice      	label;
	bool              	isSystemFont;
	qstring           	family;
	qstring           	style;
	int               	size;
	BFont             	savedFont;
	float             	familyLeft;
	float             	styleLeft;
	float             	sizeLeft;
	BTextControl*     	sizeEditor;
	static const float	sizeWidth;
};


#endif
