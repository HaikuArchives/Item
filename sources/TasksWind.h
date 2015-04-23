/* TasksWind.h */

#ifndef _H_TasksWind_
#define _H_TasksWind_

#include <Window.h>
#include <Locker.h>

class TasksView;
class Task;

class TasksWind : public BWindow {
public:
	TasksWind();
	~TasksWind();
	void             	Invalidate();
	void             	AddTask(Task* task);
	void             	RemoveTask(Task* task);
	void             	FrameMoved(BPoint screenPoint);
	static TasksWind*	GetTasksWind();
	static bool      	HaveTasksWind();
	int              	NumTasks();
	Task*            	TaskAt(int index);

protected:
	BList*            	tasks;
	TasksView*        	tasksView;
	float             	windBottom;
	bool              	growsUp;
	static TasksWind* 	tasksWind;
	static BLocker    	tasksWindCreationGuard;
	static const float	defaultWidth;
	static const float	defaultHeight;
	static const float	screenMargin;

public:
	static const float	taskHeight;
};


#endif
