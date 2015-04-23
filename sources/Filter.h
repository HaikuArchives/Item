/* Filter.h */

#ifndef _H_Filter_
#define _H_Filter_

#include "string_slice.h"
#include <Point.h>
#include <Rect.h>
#include <GraphicsDefs.h>

class qstring;
class Article;
class FiltersView;
class TextReader;
class BFile;

enum FilterAction {
	Hilite,
	Kill
};

enum FilterHeader {
	BySubject,
	ByAuthor
};

class Filter {
public:
	Filter();
	Filter(FilterAction actionIn, FilterHeader headerIn, string_slice stringIn);
	~Filter();
	void        	ApplyTo(Article* article, string_slice subject);
	void        	WriteToFile(BFile* file);
	void        	ReadFrom(TextReader* reader);
	string_slice	GetString();
	void        	SetString(string_slice newStr);
	void        	Draw(BRect rect, FiltersView* view);
	void        	MouseDown(BPoint where, BRect rect, int clicks, int modifiers, FiltersView* view);
	void        	EditString(BRect rect, FiltersView* view);
	int         	IndexFor(string_slice str, const char** strings);

protected:
	FilterAction          	action;
	FilterHeader          	header;
	qstring*              	str;
	float                 	actionRight;
	float                 	headerLeft;
	float                 	headerRight;
	float                 	strLeft;
	static const float    	textLeft;
	static const float    	baseline;
	static const rgb_color	bgndColor;
	static const float    	strEditXOffset;
	static const float    	strEditYOffset;
	static const float    	strEditRightMargin;
	static const char*    	actionStrings[];
	static const char*    	headerStrings[];
};


#endif
