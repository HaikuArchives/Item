/* TasksView.h */

#ifndef _H_TasksView_
#define _H_TasksView_

#include <View.h>

class Task;

class TasksView : public BView {
public:
	TasksView(BRect frame);
	void	Draw(BRect updateRect);
	void	DrawFiniteTask(Task* task, BRect taskRect);
	void	DrawIndeterminiteTask(Task* task, BRect taskRect);

protected:
	static const rgb_color	progressColor;
	static const rgb_color	stringColor;
	static const rgb_color	unconnectedStringColor;
	static const rgb_color	bgndColor;
	static const float    	stringLeft;
	static const float    	sliceWidth;
};


#endif
