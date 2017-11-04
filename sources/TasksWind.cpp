/* TasksWind.cpp */

#include "TasksWind.h"
#include "TasksView.h"
#include "Preferences.h"
#include <Screen.h>
#include <Autolock.h>

TasksWind* TasksWind::tasksWind = NULL;
BLocker TasksWind::tasksWindCreationGuard;
const float TasksWind::defaultWidth = 300;
const float TasksWind::defaultHeight = 60;
const float TasksWind::screenMargin = 6;
const float TasksWind::taskHeight = 12;


TasksWind::TasksWind()
	: BWindow(BRect(0, 0, defaultWidth, taskHeight - 1),		// why -1?
	          "Tasks",
	          B_FLOATING_WINDOW_LOOK, B_NORMAL_WINDOW_FEEL,
	          B_NOT_CLOSABLE | B_NOT_V_RESIZABLE | B_NOT_ZOOMABLE | B_AVOID_FRONT | B_AVOID_FOCUS),
	  growsUp(true)
{
	tasks = new BList();

	// make the tasks view
	tasksView = new TasksView(BRect(0, 0, defaultWidth, defaultHeight));
	AddChild(tasksView);

	// move to bottom right corner of screen
	BRect screenRect = BScreen(this).Frame();
	windBottom = screenRect.bottom - Frame().Height() - screenMargin;
	MoveTo(screenRect.right - Frame().Width() - screenMargin, windBottom);

	// restore the saved frame
	BRect savedFrame = Prefs()->GetRectPref("tasksWind.frame");
	if (savedFrame.IsValid() && screenRect.Contains(savedFrame)) {
		MoveTo(savedFrame.LeftTop());
		ResizeTo(savedFrame.Width(), Frame().Height());
		}

	Show();		// must do this to start the message loop
	Hide();		// until something happens
}


TasksWind::~TasksWind()
{
	Prefs()->SetRectPref("tasksWind.frame", Frame());
	tasksWind = NULL;
}


void TasksWind::Invalidate()
{
	Lock();
	tasksView->Invalidate();
	Unlock();
}


void TasksWind::AddTask(Task* task)
{
	tasks->AddItem(task);

	// adjust window
	if (IsHidden())
		Show();
	else {
		// resize
		float newHeight = tasks->CountItems() * taskHeight - 1;	// why -1?
		ResizeTo(Frame().Width(), newHeight);
		if (growsUp)
			MoveTo(Frame().left, windBottom - newHeight);
		}
}


void TasksWind::RemoveTask(Task* task)
{
	tasks->RemoveItem(task);

	// adjust window
	if (tasks->CountItems() == 0)
		Hide();
	else {
		// resize
		float newHeight = tasks->CountItems() * taskHeight - 1;	// why -1?
		ResizeTo(Frame().Width(), newHeight);
		if (growsUp)
			MoveTo(Frame().left, windBottom - newHeight);
		}
}


void TasksWind::FrameMoved(BPoint screenPoint)
{
	windBottom = Frame().bottom;
	BRect screenRect = BScreen(this).Frame();
	growsUp = (windBottom > (screenRect.top + screenRect.bottom) / 2);
}


TasksWind* TasksWind::GetTasksWind()
{
	BAutolock locker(tasksWindCreationGuard);
	if (tasksWind == NULL)
		tasksWind = new TasksWind();
	return tasksWind;
}


bool TasksWind::HaveTasksWind()
{
	return (tasksWind != NULL);
}


int TasksWind::NumTasks()
{
	return tasks->CountItems();
}


Task* TasksWind::TaskAt(int index)
{
	return (Task*) tasks->ItemAt(index);
}


