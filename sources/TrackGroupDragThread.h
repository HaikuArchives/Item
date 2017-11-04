/* TrackGroupDragThread.h */

#ifndef _H_TrackGroupDragThread_
#define _H_TrackGroupDragThread_

#include "Thread.h"

#include <Message.h>

class ScrolledListView;
class Newsrc;

class TrackGroupDragThread : public Thread {
public:
	TrackGroupDragThread(ScrolledListView* viewIn, Newsrc* newsrcIn, const BMessage* messageIn);
	~TrackGroupDragThread();
	void	Action();
	void	InsertDragAt(int index);

protected:
	ScrolledListView*	view;
	Newsrc*          	newsrc;
	BMessage*        	message;
};


#endif
