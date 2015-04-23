/* ListableTree.cpp */

#include "ListableTree.h"
#include "AATree.h"
#include "ListableObject.h"
#include "ListableTreeNode.h"


ListableTree::ListableTree()
	: AATree()
{
}


int ListableTree::DisplayHeight()
{
	return ((ListableTreeNode*) root)->displayHeight;
}


int ListableTree::IndexForY(int y)
{
	int index = 0;
	AATreeNode* node = root;
	while (node != nullNode) {
		unsigned int leftHeight =
			(node->left == nullNode ? 0 :
			 ((ListableTreeNode*) node->left)->displayHeight);
		if ((unsigned int) y < leftHeight)
			node = node->left;
		else {
			y -= leftHeight;
			index += node->left->size;
			int objectHeight = ((ListableObject*) node->object)->DisplayHeight();
			if (y < objectHeight)
				return index;
			else {
				y -= objectHeight;
				index += 1;
				node = node->right;
				}
			}
		}
	// didn't find it
	return -1;
}


int ListableTree::YAtIndex(int index)
{
	int y = 0;
	AATreeNode* node = root;
	while (node != nullNode) {
		unsigned int leftSize = node->left->size;
		if ((unsigned int) index < leftSize)
			node = node->left;
		else {
			index -= leftSize;
			if (node->left != nullNode)
				y += ((ListableTreeNode*) node->left)->displayHeight;
			if (index == 0)
				return y;
			else {
				index -= 1;
				y += ((ListableObject*) node->object)->DisplayHeight();
				node = node->right;
				}
			}
		}
	return -1;	// didn't find it
}


int ListableTree::YForObject(OrderedObject* object)
{
	int y = 0;
	AATreeNode* node = root;
	while (node != nullNode) {
		if (*object < *node->object)
			node = node->left;
		else {
			if (node->left != nullNode)
				y += ((ListableTreeNode*) node->left)->displayHeight;
			if (*object == *node->object)
				return y;
			else {
				y += ((ListableObject*) node->object)->DisplayHeight();
				node = node->right;
				}
			}
		}
	return -1;	// didn't find it
}


int ListableTree::NumSelections()
{
	if (root == nullNode)
		return 0;
	return ((ListableTreeNode*) root)->numSelections;
}


ListableObject* ListableTree::SelectedObjectAt(int index)
{
	ListableTreeNode* node = (ListableTreeNode*) root;
	while (node != nullNode) {
		ListableTreeNode* leftNode = (ListableTreeNode*) node->left;
		int leftSelections = (leftNode == nullNode ? 0 : leftNode->numSelections);
		if (index < leftSelections)
			node = leftNode;
		else {
			if (leftNode != nullNode)
				index -= leftNode->numSelections;
			bool selected = ((ListableObject*) node->object)->Selected();
			if (index == 0 && selected)
				return (ListableObject*) node->object;
			else {
				index -= selected;
				node = (ListableTreeNode*) node->right;
				}
			}
		}
	return NULL;
}


ListableObject* ListableTree::FirstSelectedObject()
{
	return SelectedObjectAt(0);
}


ListableObject* ListableTree::LastSelectedObject()
{
	return SelectedObjectAt(NumSelections() - 1);
}


void ListableTree::SelectObject(ListableObject* object)
{
	object->SetSelected(true);
	ObjectChanged(object);
}


void ListableTree::DeselectObject(ListableObject* object)
{
	object->SetSelected(false);
	ObjectChanged(object);
}


void ListableTree::Select(int index)
{
	SelectObject((ListableObject*) ObjectAt(index));
}


void ListableTree::Deselect(int index)
{
	DeselectObject((ListableObject*) ObjectAt(index));
}


void ListableTree::SelectAllObjects()
{
	SelectAllAt(root);
}


void ListableTree::DeselectAllObjects()
{
	DeselectAllAt(root);
}


void ListableTree::AllHeightsChanged()
{
	AdjustAllAt(root);
}


AATreeNode* ListableTree::MakeNode(OrderedObject* object)
{
	return new ListableTreeNode((ListableObject*) object);
}


void ListableTree::AdjustNode(AATreeNode* nodeIn)
{
	ListableTreeNode* node = (ListableTreeNode*) nodeIn;
	ListableTreeNode* left = (ListableTreeNode*) node->left;
	ListableTreeNode* right = (ListableTreeNode*) node->right;

	ListableObject* object = (ListableObject*) node->object;
	node->displayHeight = object->DisplayHeight();
	node->numSelections = object->Selected();
	if (left != nullNode) {
		node->displayHeight += left->displayHeight;
		node->numSelections += left->numSelections;
		}
	if (right != nullNode) {
		node->displayHeight += right->displayHeight;
		node->numSelections += right->numSelections;
		}
}


void ListableTree::SelectAllAt(AATreeNode* nodeIn)
{
	if (nodeIn == nullNode)
		return;

	ListableTreeNode* node = (ListableTreeNode*) nodeIn;
	if (node->left != nullNode) {
		ListableTreeNode* leftNode = (ListableTreeNode*) node->left;
		if (leftNode->numSelections < leftNode->size)
			SelectAllAt(leftNode);
		}
	if (node->right != nullNode) {
		ListableTreeNode* rightNode = (ListableTreeNode*) node->right;
		if (rightNode->numSelections < rightNode->size)
			SelectAllAt(rightNode);
		}
	((ListableObject*) node->object)->SetSelected(true);
	AdjustNode(node);
}


void ListableTree::DeselectAllAt(AATreeNode* nodeIn)
{
	if (nodeIn == nullNode)
		return;

	ListableTreeNode* node = (ListableTreeNode*) nodeIn;
	if (node->left != nullNode && ((ListableTreeNode*) node->left)->numSelections > 0)
		DeselectAllAt(node->left);
	if (node->right != nullNode && ((ListableTreeNode*) node->right)->numSelections > 0)
		DeselectAllAt(node->right);
	((ListableObject*) node->object)->SetSelected(false);
	AdjustNode(node);
}


void ListableTree::AdjustAllAt(AATreeNode* nodeIn)
{
	if (nodeIn == nullNode)
		return;

	ListableTreeNode* node = (ListableTreeNode*) nodeIn;
	if (node->left != nullNode)
		AdjustAllAt(node->left);
	if (node->right != nullNode)
		AdjustAllAt(node->right);
	AdjustNode(node);
}


