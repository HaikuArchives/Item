/* SubjectSubjectTree.cpp */

#include "SubjectSubjectTree.h"
#include "Subject.h"

class SubjectSubjectAdaptor : public SubjectTreeAdaptor {
public:
	SubjectSubjectAdaptor(Subject* subjectIn)
		: SubjectTreeAdaptor(subjectIn) {}
	bool	operator==(const OrderedObject& otherObject)
	    		{ return subject->GetSubject() == ((SubjectSubjectAdaptor*) &otherObject)->subject->GetSubject(); }
	bool	operator<(const OrderedObject& otherObject)
	    		{ return subject->GetSubject() < ((SubjectSubjectAdaptor*) &otherObject)->subject->GetSubject(); }
};


class SubjectSearchAdaptor : public OrderedObject {
public:
	SubjectSearchAdaptor(string_slice subjectIn)
		: subject(subjectIn) {}
	bool	operator==(const OrderedObject& otherObject)
	    		{ return subject == ((SubjectSubjectAdaptor*) &otherObject)->subject->GetSubject(); }
	bool	operator<(const OrderedObject& otherObject)
	    		{ return subject < ((SubjectSubjectAdaptor*) &otherObject)->subject->GetSubject(); }

protected:
	string_slice	subject;
};


SubjectTreeAdaptor* SubjectSubjectTree::MakeAdaptor(Subject* subject)
{
	return new SubjectSubjectAdaptor(subject);
}


Subject* SubjectSubjectTree::Find(string_slice subject)
{
	SubjectSearchAdaptor adaptor(subject);
	OrderedObject* foundObj = FindObject(&adaptor);
	return (foundObj ? ((SubjectSubjectAdaptor*) foundObj)->subject : NULL);
}


