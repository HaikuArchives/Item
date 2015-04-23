/* SubjectTree.h */

#ifndef _H_SubjectTree_
#define _H_SubjectTree_

#include "ListableTree.h"
#include "OrderedObject.h"
#include "SubjectTreeAdaptor.h"

class Subject;

/***
class SubjectTreeAdaptor : public OrderedObject {
public:
	SubjectTreeAdaptor(Subject* subjectIn)
		: subject(subjectIn) {}

	Subject* subject;
};
***/

class SubjectTree : public ListableTree {
public:
	virtual~SubjectTree();
	void                       	Insert(Subject* subject);
	void                       	Remove(Subject* subject);
	void                       	Walk(void (*f)(Subject* subject, void* data), void* data);
	inline unsigned int        	NumSubjects();
	Subject*                   	SubjectAt(unsigned int index);
	Subject*                   	SelectedSubjectAt(int index);
	virtual SubjectTreeAdaptor*	AdaptorFor(Subject* subject);
	virtual SubjectTreeAdaptor*	MakeAdaptor(Subject* subject);
	static void                	NodeWalker(OrderedObject* object, void* data);
};

inline unsigned int SubjectTree::NumSubjects()
{
	return NumObjects();
}




#endif
