#include"singlyLinkedListNode.h"

singlyLinkedListNode::singlyLinkedListNode() {
	this->data = nullptr;
	this->next = nullptr;
}

singlyLinkedListNode::singlyLinkedListNode(imgBlock* data) {
	this->data = data;
	this->next = nullptr;
}

singlyLinkedListNode::~singlyLinkedListNode() {
	delete this->data;
}
