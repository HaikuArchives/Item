/* Newsrc.h */

#ifndef _H_Newsrc_
#define _H_Newsrc_

#include "string_slice.h"
#include <SupportDefs.h>

class Newsgroup;
class NewsgroupsView;
class BFile;
class BList;

class Newsrc {
public:
	Newsrc(BFile* fileIn);
	~Newsrc();
	void      	Read();
	void      	Save();
	void      	NewsgroupChanged(Newsgroup* newsgroup);
	void      	AttachedToView(NewsgroupsView* viewIn);
	int       	NumNewsgroups();
	Newsgroup*	NewsgroupAt(int index);
	Newsgroup*	RemoveNewsgroupAt(int index);
	void      	AddNewsgroupAt(Newsgroup* newsgroup, int index);
	Newsgroup*	NewsgroupNamed(string_slice groupName);
	void      	ParseLine(string_slice line, string_slice* groupName, string_slice* articleSet);

protected:
	BFile*         	file;
	BList*         	newsgroups;
	NewsgroupsView*	view;
};


#endif
