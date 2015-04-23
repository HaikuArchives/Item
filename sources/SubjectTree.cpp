/* SubjectTree.cpp */

#include "SubjectTree.h"

struct WalkParams {
	void	(*f)(Subject* subject, void* data);
	void*	data;

	WalkParams(void (*fIn)(Subject* subject, void* data), void* dataIn)
		: f(fIn), data(dataIn) {}
};


SubjectTree::~SubjectTree()
{
	DeleteAllObjects();		// will only delete the adaptors, not the Subjects
}


void SubjectTree::Insert(Subject* subject)
{
	InsertObject(MakeAdaptor(subject));
}


void SubjectTree::Remove(Subject* subject)
{
	RemoveObject(AdaptorFor(subject));
}


void SubjectTree::Walk(void(*f)(Subject* subject, void* data), void* data)
{
	WalkParams params(f, data);
	WalkObjects(NodeWalker, &params);
}


Subject* SubjectTree::SubjectAt(unsigned int index)
{
	return ((SubjectTreeAdaptor*) ObjectAt(index))->subject;
}


Subject* SubjectTree::SelectedSubjectAt(int index)
{
	return ((SubjectTreeAdaptor*) SelectedObjectAt(index))->subject;
}


SubjectTreeAdaptor* SubjectTree::AdaptorFor(Subject* subject)
{
	/** pure virtual **/
	SubjectTreeAdaptor* searchAdaptor = MakeAdaptor(subject);
	SubjectTreeAdaptor* foundAdaptor = (SubjectTreeAdaptor*) FindObject(searchAdaptor);
	delete searchAdaptor;
	return foundAdaptor;
}


SubjectTreeAdaptor* SubjectTree::MakeAdaptor(Subject* subject)
{
	/** pure virtual **/
	return NULL;
}


void SubjectTree::NodeWalker(OrderedObject* object, void* data)
{
	WalkParams* params = (WalkParams*) data;
	params->f(((SubjectTreeAdaptor*) object)->subject, params->data);
}


