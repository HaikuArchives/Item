/* NewsrcWind.h */

#ifndef _H_NewsrcWind_
#define _H_NewsrcWind_

#include <Window.h>
#include "ThreadOwner.h"
#include "string_slice.h"

class Newsrc;
class NewsgroupsView;
class GetGroupStatsThread;

class NewsrcWind : public BWindow, public ThreadOwner {
public:
	NewsrcWind(Newsrc* newsrc, string_slice fileName);
	~NewsrcWind();
	void	InvalidateNewsgroupAt(int index);
	void	Quit();
	void	FrameMoved(BPoint screenPoint);
	void	FrameResized(float width, float height);
	void	SavePosition();

protected:
	NewsgroupsView*   	listView;
	static const float	defaultWidth;
	static const float	defaultHeight;
	static const float	defaultLeft;
	static const float	defaultTop;
};


#endif
