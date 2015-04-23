/* AATree.h */

#ifndef _H_AATree_
#define _H_AATree_


class OrderedObject;
class AATreeNode;

class AATree {
public:
	AATree();
	virtual~AATree();
	void               	InsertObject(OrderedObject* object);
	OrderedObject*     	RemoveObject(OrderedObject* compareObject);
	OrderedObject*     	FindObject(OrderedObject* object);
	void               	WalkObjects(void (*f)(OrderedObject* object, void* data), void* data);
	unsigned int       	NumObjects();
	OrderedObject*     	ObjectAt(unsigned int index);
	unsigned int       	IndexOf(OrderedObject* object);
	void               	DeleteAllObjects();
	void               	ObjectChanged(OrderedObject* object);
	void               	UpdateAllObjects();
	virtual AATreeNode*	MakeNode(OrderedObject* object);
	virtual void       	AdjustNode(AATreeNode* node);
	void               	InsertNodeAt(AATreeNode* newNode, AATreeNode*& atNode);
	OrderedObject*     	RemoveNodeFrom(OrderedObject* compareObject, AATreeNode** nodeHolder);
	void               	Skew(AATreeNode*& node);
	void               	Split(AATreeNode*& node);
	void               	FreeTree(AATreeNode* node);
	void               	WalkNodesAt(void (*f)(OrderedObject* object, void* data), void* data, AATreeNode* node);
	void               	ObjectChangedIn(AATreeNode* node, OrderedObject* object);
	void               	UpdateAllObjectsIn(AATreeNode* node);
	static void        	DeleteObject(OrderedObject* object, void* data);

protected:
	AATreeNode*	root;
	AATreeNode*	nullNode;
	AATreeNode*	deleteNode;
	AATreeNode*	lastNode;
};


#endif
