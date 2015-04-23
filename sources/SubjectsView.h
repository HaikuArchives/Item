/* SubjectsView.h */

#ifndef _H_SubjectsView_
#define _H_SubjectsView_

#include "AATreeListView.h"
#include "PrefsChangeClient.h"

class Subject;
class Article;

enum SelectionDirection {
	None,
	Up,
	Down
};

class SubjectsView : public AATreeListView, public PrefsChangeClient {
public:
	SubjectsView(BRect frame, ListableTree* tree, const char* name = NULL,
             uint32 resizingMode = B_FOLLOW_ALL_SIDES,
             uint32 flags = 0);
	void              	KeyDown(const char* bytes, int32 numBytes);
	void              	PrefChanged(string_slice prefName);
	SelectionDirection	SelectDirection();
	void              	SetSelectDirection(SelectionDirection newSelectDirection);
	void              	InvalidateSubject(Subject* subject);
	void              	OpenArticle(Article* article, Subject* subject);
	void              	OpenNextUnread();

protected:
	SelectionDirection	selectDirection;
};


#endif
