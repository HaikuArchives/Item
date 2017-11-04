/* ServerDlog.h */

#ifndef _H_ServerDlog_
#define _H_ServerDlog_

#include <Window.h>
#include <TextControl.h>

class ServerDlog : public BWindow {
public:
	ServerDlog();
	~ServerDlog();
	void	MessageReceived(BMessage* message);
	void	DispatchMessage(BMessage* message, BHandler* handler);
	bool	QuitRequested();
	bool	Go();

protected:
	BTextControl*         	serverControl;
	sem_id                	doneSem;
	bool                  	wasDone;
	static const float    	WindWidth;
	static const float    	WindHeight;
	static const float    	ButtonWidth;
	static const float    	ButtonHeight;
	static const float    	ButtonSpacing;
	static const float    	Margin;
	static const float    	LabelHeight;
	static const float    	ServerControlInset;
	static const rgb_color	BgndColor;
};


#endif
