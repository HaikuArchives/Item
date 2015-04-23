/* GetGroupStatsThread.h */

#ifndef _H_GetGroupStatsThread_
#define _H_GetGroupStatsThread_

#include "OwnedThread.h"

class Newsrc;
class NewsrcWind;

class GetGroupStatsThread : public OwnedThread {
public:
	GetGroupStatsThread(Newsrc* newsrcIn, NewsrcWind* windIn);
	void	Action();

protected:
	Newsrc*    	newsrc;
	NewsrcWind*	wind;
};


#endif
