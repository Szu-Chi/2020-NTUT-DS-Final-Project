#include"singlyLinkedList.h"

bool isSimilar(imgBlock* imgB1, imgBlock* imgB2, double threshold = 70) {
	double mse = imgBlock::computeMSE(imgB1->getKeyMat(), imgB2->getKeyMat());
	return mse < threshold;
}

singlyLinkedList::singlyLinkedList() {
	this->head = nullptr;
	this->size = 0;
}

singlyLinkedList::~singlyLinkedList() {
	while (this->head != nullptr) {
		singlyLinkedListNode* tmp = this->head;
		this->head = this->head->next;
		delete tmp;
	}
}

void singlyLinkedList::pushBack(imgBlock* data) {
	singlyLinkedListNode* cur = this->head;
	if (this->head == nullptr) {
		this->head = new singlyLinkedListNode(data);
		this->size++;
	}
	else {
		while (cur != nullptr) {
			if (isSimilar(cur->data, data)) {
				//TO-DO
				break;
			}
			else if (cur->next != nullptr) {
				cur = cur->next;
			}
			else {
				cur->next = new singlyLinkedListNode(data);
				this->size++;
				break;
			}
		}
	}
}

imgBlock* singlyLinkedList::search(imgBlock* data) {
	singlyLinkedListNode* cur = this->head;
	while (cur != nullptr) {
		if (isSimilar(cur->data, data)) {
			return data;
		}
		cur = cur->next;
	}
	return nullptr;
}

int singlyLinkedList::listSize() {
	return this->size;
}
