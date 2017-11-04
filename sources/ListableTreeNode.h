/* ListableTreeNode.h */

#ifndef _H_ListableTreeNode_
#define _H_ListableTreeNode_

#include "AATreeNode.h"
#include "ListableObject.h"

class ListableTreeNode : public AATreeNode {
public:
	inline	ListableTreeNode(ListableObject* object);

	int         	displayHeight;
	unsigned int	numSelections;
};

inline ListableTreeNode::ListableTreeNode(ListableObject* object)
	: AATreeNode(object), displayHeight(object->DisplayHeight()), numSelections(0)
{
}




#endif
