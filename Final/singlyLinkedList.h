#ifndef SINGLYLINKEDLIST_H
#define SINGLYLINKEDLIST_H
#include"singlyLinkedListNode.h"
class singlyLinkedList {
private:
	singlyLinkedListNode* head;
	int size;
public:
	singlyLinkedList ();
	~singlyLinkedList ();
	void pushBack(imgBlock data,int bucketIdx);
	imgBlock search(imgBlock data);
	int listSize();
	void print(int i);
};

#endif // !SINGLYLINKEDLIST_H

