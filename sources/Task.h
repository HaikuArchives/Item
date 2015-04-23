/* Task.h */

#ifndef _H_Task_
#define _H_Task_

#include "qstring.h"
#include "string_slice.h"

class Task {
public:
	Task(string_slice initProgressString);
	Task();
	~Task();
	void        	SetProgressMax(int newProgressMax);
	void        	SetProgress(int curProgress);
	void        	BumpProgress(int increment = 1);
	void        	SetProgressString(string_slice newProgressString);
	void        	SetHasConnection(bool newHasConnection);
	void        	GotConnection();
	void        	ReleasedConnection();
	inline int  	CurProgress();
	int         	GetProgressMax();
	float       	ProgressRatio();
	string_slice	ProgressString();
	bool        	HasConnection();
	bool        	IsReadyForUpdate();
	void        	SetupProgressUpdateInterval();

protected:
	qstring	progressString;
	int    	progressMax;
	int    	progressCurrent;
	bool   	hasConnection;
	int    	progressUpdateInterval;
	int    	unitsLeftUntilUpdate;
};

inline int Task::CurProgress()
{
	return progressCurrent;
}




#endif
