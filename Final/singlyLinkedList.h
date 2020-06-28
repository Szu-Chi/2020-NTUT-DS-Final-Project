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
	void pushBack(imgBlock data);
	imgBlock search(imgBlock data);
	int listSize();
	void save(int i, std::fstream& file);
};
#endif // !SINGLYLINKEDLIST_H

