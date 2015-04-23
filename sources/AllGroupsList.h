/* AllGroupsList.h */

#ifndef _H_AllGroupsList_
#define _H_AllGroupsList_

#include "GroupsList.h"

class AllGroupsList : public GroupsList {
public:
	AllGroupsList();
	~AllGroupsList();
	status_t   	ReadFile();
	void       	Save();
	void       	Dirtify();
	static void	WriteGroup(OrderedObject* object, void* data);

protected:
	char*	fileText;
	bool 	dirty;
};


#endif
