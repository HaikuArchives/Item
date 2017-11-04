/* FontPrefControl.cpp */

#include "FontPrefControl.h"
#include "FontUtils.h"
#include "Preferences.h"
#include "Messages.h"
#include <Message.h>
#include <PopUpMenu.h>
#include <MenuItem.h>
#include <TextControl.h>

const float FontPrefControl::sizeWidth = 60;


FontPrefControl::FontPrefControl(const char* prefNameIn, string_slice labelIn, BRect frame, uint32 resizingMode)
	: BView(frame, prefNameIn, resizingMode, B_WILL_DRAW),
	  prefName(prefNameIn), label(labelIn), size(12), sizeEditor(NULL)
{
	const char* defaultFont = "be_plain_font";
	string_slice prefNameSlice = prefName;
	if (prefNameSlice == "listFont")
		defaultFont = defaultListFont;
	else if (prefNameSlice == "extraInfoFont")
		defaultFont = defaultExtraInfoFont;
	FontUtils::StringToFont(Prefs()->GetStringPref(prefName, defaultFont),
	                        &savedFont, &isSystemFont);
	if (isSystemFont)
		FontUtils::FontToString(&savedFont, &family);
	else {
		font_family fontFamily;
		font_style fontStyle;
		savedFont.GetFamilyAndStyle(&fontFamily, &fontStyle);
		family = fontFamily;
		style = fontStyle;
		size = (int) savedFont.Size();
		}
}


FontPrefControl::~FontPrefControl()
{
	// revert to the saved font
	qstring fontString;
	FontUtils::FontToString(&savedFont, &fontString);
	Prefs()->SetStringPref(prefName, fontString.c_str());
}


void FontPrefControl::Save()
{
	if (isSystemFont && family == "be_plain_font")
		savedFont = *be_plain_font;
	else if (isSystemFont && family == "be_bold_font")
		savedFont = *be_bold_font;
	else if (isSystemFont && family == "be_fixed_font")
		savedFont = *be_fixed_font;
	else {
		savedFont.SetFamilyAndStyle(family.c_str(), style.c_str());
		savedFont.SetSize(size);
		}
}


void FontPrefControl::SetPref()
{
	BFont font;
	if (isSystemFont && family == "be_plain_font")
		font = *be_plain_font;
	else if (isSystemFont && family == "be_bold_font")
		font = *be_bold_font;
	else if (isSystemFont && family == "be_fixed_font")
		font = *be_fixed_font;
	else {
		font.SetFamilyAndStyle(family.c_str(), style.c_str());
		font.SetSize(size);
		}
	qstring fontString;
	FontUtils::FontToString(&font, &fontString);
	Prefs()->SetStringPref(prefName, fontString.c_str());
}


void FontPrefControl::Draw(BRect updateRect)
{
	// set up
	SetFont(be_bold_font);
	SetLowColor(ViewColor());
	font_height fontInfo;
	GetFontHeight(&fontInfo);
	MovePenTo(0, ceil(fontInfo.ascent));

	// label
	DrawString(label.begin(), label.length());
	DrawString("  ");

	// family
	SetFont(be_plain_font);
	familyLeft = PenLocation().x;
	string_slice familyName = family;
	if (familyName == "be_plain_font")
		familyName = "Plain Font";
	else if (familyName == "be_bold_font")
		familyName = "Bold Font";
	else if (familyName == "be_fixed_font")
		familyName = "Fixed Font";
	DrawString(familyName.begin(), familyName.length());
	DrawString(" ");

	if (isSystemFont)
		return;

	// style
	styleLeft = PenLocation().x;
	DrawString(style.data(), style.length());
	DrawString(" ");

	// size
	sizeLeft = PenLocation().x;
	qstring sizeStr(size);
	DrawString(sizeStr.data(), sizeStr.length());
}


void FontPrefControl::GetPreferredSize(float* width, float* height)
{
	*width = Bounds().Width();
	font_height fontInfo;
	be_bold_font->GetHeight(&fontInfo);
	*height = ceil(fontInfo.ascent + fontInfo.descent + fontInfo.leading);
}


void FontPrefControl::MouseDown(BPoint point)
{
	if (!isSystemFont && point.x >= sizeLeft + sizeWidth) {
		// do nothing
		}

	// size
	else if (!isSystemFont && point.x >= sizeLeft)
		StartSizeEdit();

	// style
	else if (!isSystemFont && point.x >= styleLeft)
		PopStyleMenu();

	else if (point.x >= familyLeft)
		PopFamilyMenu();
}


void FontPrefControl::MessageReceived(BMessage* message)
{
	switch (message->what) {
		case EditDoneMessage:
			FinishSizeEdit();
			break;

		default:
			BView::MessageReceived(message);
			break;
		}
}


void FontPrefControl::PopStyleMenu()
{
	// build the menu
	BPopUpMenu* menu = new BPopUpMenu("Style");
	int numStyles = count_font_styles((char*) family.c_str());	// casting away "const" because count_font_styles() isn't declared right
	for (int i=0; i<numStyles; i++) {
		font_style curStyle;
		get_font_style((char*) family.c_str(), i, &curStyle);
		BMenuItem* item = new BMenuItem(curStyle, NULL);
		item->SetMarked(style == curStyle);
		menu->AddItem(item);
		}

	// pop up the menu
	BPoint popPoint = ConvertToScreen(BPoint(styleLeft, 0));
	BMenuItem* selectedItem = menu->Go(popPoint);

	// get the result
	if (selectedItem)
		style = selectedItem->Label();

	// clean up
	delete menu;
	SetPref();
	Invalidate();
}


void FontPrefControl::PopFamilyMenu()
{
	int i;

	// build the menu
	BPopUpMenu* menu = new BPopUpMenu("Style");
	// system fonts
	BMenuItem* item = new BMenuItem("Plain Font", NULL);
	item->SetMarked(isSystemFont && family == "be_plain_font");
	menu->AddItem(item);
	item = new BMenuItem("Bold Font", NULL);
	item->SetMarked(isSystemFont && family == "be_bold_font");
	menu->AddItem(item);
	item = new BMenuItem("Fixed Font", NULL);
	item->SetMarked(isSystemFont && family == "be_fixed_font");
	menu->AddItem(item);
	menu->AddSeparatorItem();
	// families
	int numFamilies = count_font_families();
	for (i=0; i<numFamilies; i++) {
		font_family curFamily;
		get_font_family(i, &curFamily);
		item = new BMenuItem(curFamily, NULL);
		item->SetMarked(family == curFamily);
		menu->AddItem(item);
		}

	// pop up the menu
	BPoint popPoint = ConvertToScreen(BPoint(familyLeft, 0));
	BMenuItem* selectedItem = menu->Go(popPoint);

	// get the result
	if (selectedItem) {
		family = selectedItem->Label();
		isSystemFont = false;
		if (family == "Plain Font") {
			family = "be_plain_font";
			isSystemFont = true;;
			}
		else if (family == "Bold Font") {
			family = "be_bold_font";
			isSystemFont = true;;
			}
		else if (family == "Fixed Font") {
			family = "be_fixed_font";
			isSystemFont = true;;
			}
		}

	// adjust style
	if (!isSystemFont) {
		// see if this family has the current style
		bool haveStyle = false;
		font_style curStyle;
		int numStyles = count_font_styles((char*) family.c_str());	// casting away "const" because count_font_styles() isn't declared right
		for (i=0; i<numStyles; i++) {
			get_font_style((char*) family.c_str(), i, &curStyle);
			if (style == curStyle) {
				haveStyle = true;
				break;
				}
			}
		// if not, use the first style in the family
		if (!haveStyle) {
			get_font_style((char*) family.c_str(), 0, &curStyle);
			style = curStyle;
			}
		}

	// clean up
	delete menu;
	SetPref();
	Invalidate();
}


void FontPrefControl::StartSizeEdit()
{
	if (sizeEditor)
		return;

	BRect rect = Bounds();
	rect.left = sizeLeft;
	rect.right = rect.left + sizeWidth;
	sizeEditor = new BTextControl(rect, "Font Size", NULL,
	                              qstring(size).c_str(),
	                              new BMessage(EditDoneMessage),
	                              B_FOLLOW_LEFT_RIGHT | B_FOLLOW_TOP);
	sizeEditor->SetTarget(this);
	AddChild(sizeEditor);
	sizeEditor->MakeFocus();
}


void FontPrefControl::FinishSizeEdit()
{
	if (sizeEditor == NULL)
		return;
	size = string_slice(sizeEditor->Text()).asInt();
	RemoveChild(sizeEditor);
	delete sizeEditor;
	sizeEditor = NULL;
	SetPref();
	Invalidate();
}


