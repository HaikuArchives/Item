/* SubjectFilterTree.cpp */

#include "SubjectFilterTree.h"
#include "Subject.h"

class SubjectFilterAdaptor : public SubjectTreeAdaptor {
public:
	SubjectFilterAdaptor(Subject* subjectIn)
		: SubjectTreeAdaptor(subjectIn) {}
	bool	operator==(const OrderedObject& otherObject);
	bool	operator<(const OrderedObject& otherObject);
};

bool SubjectFilterAdaptor::operator==(const OrderedObject& otherObject)
{
	Subject* otherSubject = ((SubjectFilterAdaptor*) &otherObject)->subject;
	if (subject->FilterScore() != otherSubject->FilterScore())
		return false;
	else if (subject->MinArticleDate() != otherSubject->MinArticleDate())
		return false;
	else
		return (subject->GetSubject() == otherSubject->GetSubject());
}

bool SubjectFilterAdaptor::operator<(const OrderedObject& otherObject)
{
	Subject* otherSubject = ((SubjectFilterAdaptor*) &otherObject)->subject;
	if (subject->FilterScore() != otherSubject->FilterScore())
		// descending order
		return (subject->FilterScore() > otherSubject->FilterScore());
	else if (subject->MinArticleDate() != otherSubject->MinArticleDate())
		return (subject->MinArticleDate() < otherSubject->MinArticleDate());
	else
		return (subject->GetSubject() < otherSubject->GetSubject());
}



SubjectTreeAdaptor* SubjectFilterTree::MakeAdaptor(Subject* subject)
{
	return new SubjectFilterAdaptor(subject);
}


