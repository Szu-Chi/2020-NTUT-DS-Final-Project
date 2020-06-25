#ifndef SINGLYLINKEDLIST_NODE_H
#define SINGLYLINKEDLIST_NODE_H
#include"imgBlock.h"

class singlyLinkedListNode {
public:
	imgBlock data;
	singlyLinkedListNode* next;
	singlyLinkedListNode();
	singlyLinkedListNode(imgBlock data);
	~singlyLinkedListNode();
};
#endif // !SINGLYLINKEDLIST_NODE_H
