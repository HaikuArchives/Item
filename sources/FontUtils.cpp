/* FontUtils.cpp */

#include "FontUtils.h"
#include "TextReader.h"
#include <Font.h>


void FontUtils::FontToString(const BFont* font, qstring* str)
{
	if (*font == *be_plain_font) {
		*str = "be_plain_font";
		return;
		}
	else if (*font == *be_bold_font) {
		*str = "be_bold_font";
		return;
		}
	else if (*font == *be_fixed_font) {
		*str = "be_fixed_font";
		return;
		}

	font_family family;
	font_style style;
	font->GetFamilyAndStyle(&family, &style);
	*str = family;
	*str += '\t';
	*str += style;
	*str += '\t';
	*str += ((int) font->Size());
}


void FontUtils::StringToFont(const string_slice str, BFont* font, bool* isSystemFont)
{
	if (str == "be_plain_font") {
		*font = *be_plain_font;
		if (isSystemFont)
			*isSystemFont = true;
		return;
		}
	else if (str == "be_bold_font") {
		*font = *be_bold_font;
		if (isSystemFont)
			*isSystemFont = true;
		return;
		}
	else if (str == "be_fixed_font") {
		*font = *be_fixed_font;
		if (isSystemFont)
			*isSystemFont = true;
		return;
		}

	TextReader parser(str);
	qstring family = parser.NextTabField();
	qstring style = parser.NextTabField();
	qstring size = parser.NextTabField();
	*font = *be_plain_font;
	font->SetFamilyAndStyle(family.c_str(), style.c_str());
	font->SetSize(size.asInt());
	if (isSystemFont)
		*isSystemFont = false;
}


