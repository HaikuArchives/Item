/* TasksView.cpp */

#include "TasksView.h"
#include "Task.h"
#include "TasksWind.h"
#include "string_slice.h"
#include <Region.h>

const rgb_color TasksView::progressColor = { 192, 192, 192, 255 };
const rgb_color TasksView::stringColor = { 0, 0, 0, 255 };
const rgb_color TasksView::unconnectedStringColor = { 226, 190, 74, 255 };
const rgb_color TasksView::bgndColor = { 255, 255, 255, 255 };
const float TasksView::stringLeft = 6;
const float TasksView::sliceWidth = 10;


TasksView::TasksView(BRect frame)
	: BView(frame, "Tasks", B_FOLLOW_ALL_SIDES,
	        B_WILL_DRAW | B_FULL_UPDATE_ON_RESIZE)
{
	SetViewColor(B_TRANSPARENT_32_BIT);
}


void TasksView::Draw(BRect updateRect)
{
	TasksWind* wind = (TasksWind*) Window();
		// I was using TasksWind::GetTaskWind() for this, but that
		// caused a deadlock once the creation guard was added to
		// prevent the race condition

	// set up initial taskRect
	BRect taskRect = Bounds();
	taskRect.bottom = taskRect.top + TasksWind::taskHeight - 1;

	// draw the connections
	int numTasks = wind->NumTasks();
	for (int i=0; i<numTasks; i++) {
		Task* task = wind->TaskAt(i);
		if (task == NULL)
			continue;

		if (task->GetProgressMax() == 0 && task->CurProgress() != 0)
			DrawIndeterminiteTask(task, taskRect);
		else
			DrawFiniteTask(task, taskRect);

		// bump the connectionRect
		taskRect.OffsetBy(0, TasksWind::taskHeight);
		}
}


void TasksView::DrawFiniteTask(Task* task, BRect taskRect)
{
	// set up to draw
	float progressRatio = task->ProgressRatio();
	string_slice progressString = task->ProgressString();
	font_height fontInfo;
	be_plain_font->GetHeight(&fontInfo);
	BPoint stringStart(taskRect.left + stringLeft,
	                   taskRect.bottom - fontInfo.descent);
	BRegion clipRegion;

	// draw the first half of the progress
	BRect progressRect = taskRect;
	progressRect.right = progressRect.left + ceil(progressRatio * progressRect.Width());
	if (progressRect.IsValid()) {
		PushState();
		clipRegion.Set(progressRect);
		ConstrainClippingRegion(&clipRegion);
		SetHighColor(task->HasConnection() ? stringColor : unconnectedStringColor);
		SetLowColor(progressColor);
		FillRect(progressRect, B_SOLID_LOW);
		if (progressString.length() > 0)
			DrawString(progressString.begin(), progressString.length(), stringStart);
		PopState();
		}

	// draw the second half of the progress
	progressRect.left = progressRect.right + 1;
	progressRect.right = taskRect.right;
	if (progressRect.IsValid()) {
		PushState();
		clipRegion.Set(progressRect);
		ConstrainClippingRegion(&clipRegion);
		SetHighColor(task->HasConnection() ? stringColor : unconnectedStringColor);
		SetLowColor(bgndColor);
		FillRect(progressRect, B_SOLID_LOW);
		if (progressString.length() > 0)
			DrawString(progressString.begin(), progressString.length(), stringStart);
		PopState();
		}
}


void TasksView::DrawIndeterminiteTask(Task* task, BRect taskRect)
{
	// set up to draw
	string_slice progressString = task->ProgressString();
	font_height fontInfo;
	be_plain_font->GetHeight(&fontInfo);
	BPoint stringStart(taskRect.left + stringLeft,
	                   taskRect.bottom - fontInfo.descent);
	BRegion clipRegion;

	// draw the slice
	// calc the slice
	int sliceMax = (int) (taskRect.Width() / sliceWidth);
	int curSlice = task->CurProgress() % (sliceMax * 2);
	if (curSlice > sliceMax)
		curSlice = sliceMax * 2 - curSlice;
	BRect sliceRect = taskRect;
	sliceRect.left += curSlice * sliceWidth;
	sliceRect.right = sliceRect.left + sliceWidth;
	// draw
	PushState();
	clipRegion.Set(sliceRect);
	ConstrainClippingRegion(&clipRegion);
	SetHighColor(task->HasConnection() ? stringColor : unconnectedStringColor);
	SetLowColor(progressColor);
	FillRect(sliceRect, B_SOLID_LOW);
	if (progressString.length() > 0)
		DrawString(progressString.begin(), progressString.length(), stringStart);
	PopState();

	// draw the rest of the progress
	// calc the clipping region
	clipRegion.MakeEmpty();
	BRect nonSliceRect = taskRect;
	nonSliceRect.right = sliceRect.left - 1;
	if (nonSliceRect.IsValid())
		clipRegion.Include(nonSliceRect);
	nonSliceRect = taskRect;
	nonSliceRect.left = sliceRect.right + 1;
	if (nonSliceRect.IsValid())
		clipRegion.Include(nonSliceRect);
	// draw
	PushState();
	ConstrainClippingRegion(&clipRegion);
	SetHighColor(task->HasConnection() ? stringColor : unconnectedStringColor);
	SetLowColor(bgndColor);
	FillRect(taskRect, B_SOLID_LOW);
	if (progressString.length() > 0)
		DrawString(progressString.begin(), progressString.length(), stringStart);
	PopState();
}


