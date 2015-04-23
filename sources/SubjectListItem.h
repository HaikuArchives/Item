/* SubjectListItem.h */

#ifndef _H_SubjectListItem_
#define _H_SubjectListItem_

#include "BaseListItem.h"

class Subject;

class SubjectListItem : public BaseListItem {
public:
	SubjectListItem(Subject* subjectIn);
	void	DrawItem(BView* view, BRect itemRect, bool drawEverything);
	bool	Open(Newsgroup* newsgroup);
	void	Mark(Newsgroup* newsgroup);
	void	Unmark(Newsgroup* newsgroup);

protected:
	Subject*          	subject;
	static const float	SubjectStart;
	static const float	MarkStart;
};


#endif
