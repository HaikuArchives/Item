/* SubjectSubjectTree.h */

#ifndef _H_SubjectSubjectTree_
#define _H_SubjectSubjectTree_

#include "SubjectTree.h"
#include "string_slice.h"

class SubjectSubjectTree : public SubjectTree {
public:
	SubjectTreeAdaptor*	MakeAdaptor(Subject* subject);
	Subject*           	Find(string_slice subject);
};


#endif
