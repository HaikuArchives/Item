/* Task.cpp */

#include "Task.h"
#include "TasksWind.h"



Task::Task(string_slice initProgressString)
	: progressString(initProgressString), progressMax(0), progressCurrent(0),
	  hasConnection(false), progressUpdateInterval(1), unitsLeftUntilUpdate(0)
{
	TasksWind::GetTasksWind()->AddTask(this);
}


Task::Task()
	: progressString("Waiting for connection..."), progressMax(0), progressCurrent(0),
	  hasConnection(false)
{
	TasksWind::GetTasksWind()->AddTask(this);
}


Task::~Task()
{
	TasksWind::GetTasksWind()->RemoveTask(this);
}


void Task::SetProgressMax(int newProgressMax)
{
	progressMax = newProgressMax;
	SetupProgressUpdateInterval();
	TasksWind::GetTasksWind()->Invalidate();
}


void Task::SetProgress(int curProgress)
{
	progressCurrent = curProgress;
	unitsLeftUntilUpdate = progressUpdateInterval;
	TasksWind::GetTasksWind()->Invalidate();
}


void Task::BumpProgress(int increment)
{
	progressCurrent += increment;
	TasksWind::GetTasksWind()->Invalidate();
}


void Task::SetProgressString(string_slice newProgressString)
{
	progressString = newProgressString;
	TasksWind::GetTasksWind()->Invalidate();
}


void Task::SetHasConnection(bool newHasConnection)
{
	hasConnection = newHasConnection;
	TasksWind::GetTasksWind()->Invalidate();
}


void Task::GotConnection()
{
	SetHasConnection(true);
}


void Task::ReleasedConnection()
{
	SetHasConnection(false);
}


int Task::GetProgressMax()
{
	return progressMax;
}


float Task::ProgressRatio()
{
	return (progressMax > 0 ? ((float) progressCurrent) / progressMax : 0);
}


string_slice Task::ProgressString()
{
	return progressString;
}


bool Task::HasConnection()
{
	return hasConnection;
}


bool Task::IsReadyForUpdate()
{
	return (--unitsLeftUntilUpdate <= 0);
}


void Task::SetupProgressUpdateInterval()
{
	// Updating the progress involves a certain amount of overhead, which we're
	// trying to avoid because it can be significant.
	// This method doesn't need to be called directly, since SetProgressMax()
	// will call it.
	// We want to update at least once per pixel, so we update 2000 times,
	// since that's wider than any known screen.
	static const unsigned long minUpdates = 2000;

	int progressUpdateInterval = progressMax / minUpdates;
	if (progressUpdateInterval < 1)
		progressUpdateInterval = 1;
	unitsLeftUntilUpdate = 0;
}


