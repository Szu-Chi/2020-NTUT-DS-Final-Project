#include"singlyLinkedList.h"
bool isSimilar(imgBlock imgB1, imgBlock imgB2, double threshold = 70) {
	double mse = imgBlock::computeMSE(imgB1.getKeyMat(), imgB2.getKeyMat());
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

void singlyLinkedList::pushBack(imgBlock data,int bucketIdx) {
	singlyLinkedListNode* cur = this->head;
	if (this->head == nullptr) {
		this->head = new singlyLinkedListNode(data);
		this->size++;
		std::cout << bucketIdx << std::endl;
	}
	else {
		while (cur != nullptr) {
			if (isSimilar(cur->data, data)) {
				cur->data += data;
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

imgBlock singlyLinkedList::search(imgBlock data) {
	singlyLinkedListNode* cur = this->head;
	while (cur != nullptr) {
		if (isSimilar(cur->data, data)) {
			return data;
		}
		cur = cur->next;
	}
	imgBlock null = imgBlock(cv::Mat(1,1,0), timeSeg(-1,-1));
	return null;
}

int singlyLinkedList::listSize() {
	return this->size;
}

void singlyLinkedList::print(int i) {
	int j = 0;
	singlyLinkedListNode* cur = this->head;
	while (cur != nullptr) {
		std::string bucketSlot = std::to_string(i) + "-" + std::to_string(j++);
		cur->data.print();
		//cv::imshow(bucketSlot, cur->data.frame);
		
		cur->data.save(bucketSlot);
		cur = cur->next;
	}
}
