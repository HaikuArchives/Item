/* AddGroupDlog.h */

#ifndef _H_AddGroupDlog_
#define _H_AddGroupDlog_

#include <Window.h>
#include <Messenger.h>
#include <TextControl.h>

class AddGroupDlog : public BWindow {
public:
	AddGroupDlog(const char* title, const BMessenger& targetIn);
	void	DispatchMessage(BMessage* message, BHandler* handler);
	void	MessageReceived(BMessage* message);

protected:
	BMessenger            	target;
	BTextControl*         	groupView;
	BButton*              	addButton;
	static const rgb_color	bgndColor;
	static const float    	margin;
	static const float    	groupViewDivider;
	static const float    	buttonWidth;
	static const float    	buttonHeight;
	static const float    	buttonHSpacing;
	static const float    	windHeight;
	static const float    	defaultWindWidth;
	static const int32    	AcceptedMessage;
};


#endif
