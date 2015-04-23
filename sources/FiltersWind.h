/* FiltersWind.h */

#ifndef _H_FiltersWind_
#define _H_FiltersWind_

#include <Window.h>

class FiltersView;

class FiltersWind : public BWindow {
public:
	FiltersWind();
	~FiltersWind();
	void	MenusBeginning();

protected:
	FiltersView*      	listView;
	static const float	defaultWidth;
	static const float	defaultHeight;
	static const float	defaultLeft;
	static const float	defaultTop;
};


#endif
