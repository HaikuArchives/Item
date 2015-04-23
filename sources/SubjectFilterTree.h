/* SubjectFilterTree.h */

#ifndef _H_SubjectFilterTree_
#define _H_SubjectFilterTree_

#include "SubjectTree.h"

class SubjectFilterTree : public SubjectTree {
public:
	SubjectTreeAdaptor*	MakeAdaptor(Subject* subject);
};


#endif
