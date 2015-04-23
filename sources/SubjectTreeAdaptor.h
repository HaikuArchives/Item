/* SubjectTreeAdaptor.h */

#ifndef _H_SubjectTreeAdaptor_
#define _H_SubjectTreeAdaptor_

#include "ListableObject.h"

class Subject;
class Article;
class Newsgroup;

class SubjectTreeAdaptor : public ListableObject {
public:
	inline     	SubjectTreeAdaptor(Subject* subjectIn);
	int        	DisplayHeight();
	bool       	Selected();
	void       	SetSelected(bool newSelected);
	void       	Draw(BRect rect, AATreeListView* view);
	void       	MouseDown(BPoint where, BRect rect, int clicks, int modifiers, AATreeListView* view);
	void       	Open(AATreeListView* view);
	void       	SelectDown(uint32 modifiers, AATreeListView* viewIn);
	void       	SelectUp(uint32 modifiers, AATreeListView* viewIn);
	void       	SelectLast(AATreeListView* view);
	static void	FontChanged();
	static void	ExtraInfoDisplayChanged();
	int        	NumSelections();
	static bool	HasExtraInfo();
	void       	DrawSubject(BRect itemRect, BView* view);
	void       	DrawArticle(int whichArticle, BRect itemRect, BView* view);
	void       	SetupColors(BView* view, BRect itemRect, bool selected, float filterScore);
	void       	DeselectAllArticles();
	void       	OpenArticle(Article* article, Newsgroup* newsgroup);
	static void	SetupFont();

	Subject*          	subject;

protected:
	static float      	itemHeight;
	static BFont      	font;
	static BFont      	extraInfoFont;
	static float      	articleItemIndent;
	static float      	linesWidth;
	static bool       	showLines;
	static bool       	showDates;
	static const float	expanderLeft;
	static const float	itemLeft;
	static const float	numArticlesRight;
	static const float	subjectLeft;
	static const float	articleItemIndentEms;
	static const float	articleMarkLeft;
	static const float	articleLeft;
	static const float	itemRight;
	static const float	baseline;
};

inline SubjectTreeAdaptor::SubjectTreeAdaptor(Subject* subjectIn)
	: subject(subjectIn)
{
}




#endif
