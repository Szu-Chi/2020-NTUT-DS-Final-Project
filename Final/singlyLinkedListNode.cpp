#include"singlyLinkedListNode.h"

singlyLinkedListNode::singlyLinkedListNode() {
	this->next = nullptr;
}

singlyLinkedListNode::singlyLinkedListNode(imgBlock data) {
	this->data = data;
	this->next = nullptr;
}

singlyLinkedListNode::~singlyLinkedListNode() {
}
