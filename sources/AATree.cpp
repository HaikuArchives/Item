/* AATree.cpp */

#include "AATree.h"
#include "OrderedObject.h"
#include "AATreeNode.h"

class NullNode : public AATreeNode {
public:
	NullNode()
		: AATreeNode(NULL) { level = size = 0; left = right = this; }
	bool	operator<(const AATreeNode& otherNode) { return false; }
	bool	operator==(const AATreeNode& otherNode) { return false; }
};



AATree::AATree()
{
	nullNode = new NullNode();
	root = nullNode;
}


AATree::~AATree()
{
	FreeTree(root);
	delete nullNode;
}


void AATree::InsertObject(OrderedObject* object)
{
	AATreeNode* newNode = MakeNode(object);
	newNode->left = newNode->right = nullNode;
	InsertNodeAt(newNode, root);
}


OrderedObject* AATree::RemoveObject(OrderedObject* compareObject)
{
	deleteNode = nullNode;
	return RemoveNodeFrom(compareObject, &root);
}


OrderedObject* AATree::FindObject(OrderedObject* object)
{
	AATreeNode* curNode = root;
	while (curNode != nullNode && *object != *curNode->object)
		curNode = (*object < *curNode->object ? curNode->left : curNode->right);
	return (curNode == nullNode ? NULL : curNode->object);
}


void AATree::WalkObjects(void(*f)(OrderedObject* object, void* data), void* data)
{
	WalkNodesAt(f, data, root);
}


unsigned int AATree::NumObjects()
{
	if (root == NULL)
		return 0;
	else
		return root->size;
}


OrderedObject* AATree::ObjectAt(unsigned int index)
{
	index += 1;		// convert from zero-based to one-based
	AATreeNode* node = root;
	while (node != nullNode) {
		unsigned int leftSize = node->left->size;
		if (index <= leftSize)
			node = node->left;
		else if (index == leftSize + 1)
			return node->object;
		else {
			index -= leftSize + 1;
			node = node->right;
			}
		}
	// didn't find it
	return NULL;
}


unsigned int AATree::IndexOf(OrderedObject* object)
{
	int index = 0;
	AATreeNode* curNode = root;
	while (curNode != nullNode) {
		if (*object == *curNode->object)
			return index + curNode->left->size;
		else if (*object < *curNode->object)
			curNode = curNode->left;
		else {
			index += curNode->left->size + 1;
			curNode = curNode->right;
			}
		}
	return (unsigned int) -1;	// didn't find it
}


void AATree::DeleteAllObjects()
{
	WalkObjects(DeleteObject, NULL);
	FreeTree(root);
	root = nullNode;
}


void AATree::ObjectChanged(OrderedObject* object)
{
	ObjectChangedIn(root, object);
}


void AATree::UpdateAllObjects()
{
	UpdateAllObjectsIn(root);
}


AATreeNode* AATree::MakeNode(OrderedObject* object)
{
	return new AATreeNode(object);
}


void AATree::AdjustNode(AATreeNode* node)
{
}


void AATree::InsertNodeAt(AATreeNode* newNode, AATreeNode*& atNode)
{
	if (atNode == nullNode)
		atNode = newNode;
	else if (*newNode->object < *atNode->object) {
		InsertNodeAt(newNode, atNode->left);
		atNode->size++;
		AdjustNode(atNode);
		}
	else {
		InsertNodeAt(newNode, atNode->right);
		atNode->size++;
		AdjustNode(atNode);
		}

	Skew(atNode);
	Split(atNode);
}


OrderedObject* AATree::RemoveNodeFrom(OrderedObject* compareObject, AATreeNode** nodeHolder)
{
	AATreeNode* node = *nodeHolder;
	if (node == nullNode)
		return NULL;

	OrderedObject* removedObject = NULL;

	// search down the tree and set lastNode and deleteNode
	lastNode = node;
	if (*compareObject < *node->object)
		removedObject = RemoveNodeFrom(compareObject, &node->left);
	else {
		deleteNode = node;
		removedObject = RemoveNodeFrom(compareObject, &node->right);
		}

	// remove if at bottom of the tree and the node is present
	if (node == lastNode) {
		// swap this node in for the deleted node
		if (deleteNode != nullNode && *compareObject == *deleteNode->object) {
			// swap in to deleted node
			removedObject = deleteNode->object;
			deleteNode->object = node->object;
			// splice out this node
			*nodeHolder = node->right;
			// delete the node
			delete node;
			}
		else {
			// mark as not found
			// (not actually necessary)
			deleteNode = nullNode;
			lastNode = NULL;
			}
		}

	// otherwise we're not at the bottom, so rebalance
	else {
		node = *nodeHolder;		// in case things got moved around
		// adjust size
		if (removedObject) {
			--node->size;
			AdjustNode(node);
			}
		// rebalance
		if (node->left->level < node->level - 1 ||
				node->right->level < node->level - 1) {
			if (node->right->level > --node->level)
				node->right->level = node->level;
			Skew(*nodeHolder);
			node = *nodeHolder;
			Skew(node->right);
			Skew(node->right->right);
			Split(*nodeHolder);
			node = *nodeHolder;
			Split(node->right);
			}
		}

	return removedObject;
}


void AATree::Skew(AATreeNode*& node)
{
	if (node->left->level == node->level && node != nullNode) {
		// (had to check for nullNode because of size adjustment; otherwise
		// this all works okay even on nullNode)
		// rotate with left child
		AATreeNode* childNode = node->left;
		node->left = childNode->right;
		childNode->right = node;
		// hoist the child node up
		node = childNode;
		// adjust sizes & display heights
		childNode = node->right;
		childNode->size = childNode->left->size + childNode->right->size + 1;
		AdjustNode(childNode);
		node->size = node->left->size + node->right->size + 1;
		AdjustNode(node);
/***
		childNode->displayHeight =
			childNode->left->displayHeight + childNode->right->displayHeight +
			(childNode->object ? childNode->object->DisplayHeight() : 0);
		node->displayHeight =
			node->left->displayHeight + node->right->displayHeight +
			(node->object ? node->object->DisplayHeight() : 0);
***/
		}
}


void AATree::Split(AATreeNode*& node)
{
	if (node->right->right->level == node->level && node != nullNode) {
		// (had to check for nullNode because of size adjustment; otherwise
		// this all works okay even on nullNode)
		// rotate with right child
		AATreeNode* childNode = node->right;
		node->right = childNode->left;
		childNode->left = node;
		// hoist the child
		node = childNode;
		node->level++;
		// adjust the sizes & displayHeights
		childNode = node->left;
		childNode->size = childNode->left->size + childNode->right->size + 1;
		node->size = node->left->size + node->right->size + 1;
		AdjustNode(childNode);
		AdjustNode(node);
/***
		childNode->displayHeight =
			childNode->left->displayHeight + childNode->right->displayHeight +
			(childNode->object ? childNode->object->DisplayHeight() : 0);
		node->displayHeight =
			node->left->displayHeight + node->right->displayHeight +
			(node->object ? node->object->DisplayHeight() : 0);
***/
		}
}


void AATree::FreeTree(AATreeNode* node)
{
	if (node != nullNode) {
		FreeTree(node->right);
		FreeTree(node->left);
		delete node;
		}
}


void AATree::WalkNodesAt(void(*f)(OrderedObject* object, void* data), void* data, AATreeNode* node)
{
	if (node != nullNode) {
		WalkNodesAt(f, data, node->left);
		f(node->object, data);
		WalkNodesAt(f, data, node->right);
		}
}


void AATree::ObjectChangedIn(AATreeNode* node, OrderedObject* object)
{
	if (node == nullNode)
		return;
	if (*object < *node->object)
		ObjectChangedIn(node->left, object);
	else if (*object != *node->object)
		ObjectChangedIn(node->right, object);
	AdjustNode(node);
}


void AATree::UpdateAllObjectsIn(AATreeNode* node)
{
	if (node == nullNode)
		return;
	UpdateAllObjectsIn(node->left);
	UpdateAllObjectsIn(node->right);
	AdjustNode(node);
}


void AATree::DeleteObject(OrderedObject* object, void* data)
{
	delete object;
}


