/* NewsgroupWind.h */

#ifndef _H_NewsgroupWind_
#define _H_NewsgroupWind_

#include <Window.h>

class Newsgroup;
class SubjectsView;
class Subject;

class NewsgroupWind : public BWindow {
public:
	NewsgroupWind(Newsgroup* newsgroupIn);
	~NewsgroupWind();
	void      	MessageReceived(BMessage* message);
	void      	MenusBeginning();
	void      	FrameMoved(BPoint screenPoint);
	void      	FrameResized(float width, float height);
	Newsgroup*	GetNewsgroup();
	void      	SubjectChanged(Subject* subject);
	void      	NumSubjectsChanged();
	void      	MarkSelected(bool marked);
	void      	AppendArticles();
	void      	SaveArticles();
	void      	ExtractBinaries(bool multipart);
	void      	SavePosition();
	void      	KillSubjects();
	void      	HiliteSubjects();

protected:
	Newsgroup*        	newsgroup;
	SubjectsView*     	listView;
	static const float	DefaultWidth;
	static const float	DefaultHeight;
	static const float	DefaultLeft;
	static const float	DefaultTop;
};


#endif
