/* AATreeNode.h */

#ifndef _H_AATreeNode_
#define _H_AATreeNode_

#include <SupportDefs.h>

class AATreeNode {
public:
	inline	AATreeNode(OrderedObject* objectIn);

	AATreeNode*   	left;
	AATreeNode*   	right;
	int           	level;
	unsigned int  	size;
	OrderedObject*	object;
};

inline AATreeNode::AATreeNode(OrderedObject* objectIn)
	: left(NULL), right(NULL), level(1), size(1), object(objectIn)
{
}




#endif
