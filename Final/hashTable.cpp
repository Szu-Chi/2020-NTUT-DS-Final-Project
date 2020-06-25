#include"hashTable.h"
hashTable::hashTable(int nBucket) {
	this->list = new singlyLinkedList[nBucket];
	this->bucketN = nBucket;
	this->bits = static_cast<int>(ceil(log2(nBucket)));
	this->segN = 64 / this->bits;
}

hashTable::~hashTable() {
	delete[]this->list;
}

uint64_t hashTable::hash(imgBlock* data) {
	uint64_t hashVal = 0;
	Mat keyMat = data->getKeyMat();
	int zeros = 0;
	int ones = 0;
	int count = this->bits;
	for (int i = 0; i < keyMat.rows; i++) {
		for (int j = 0; j < keyMat.cols; j++) {
			uchar* pixel = keyMat.ptr<uchar>(i, j);
			(int(*pixel) == 255) ? ones++ : zeros++;
			if (ones + zeros == this->segN && count != 1) {
				if (ones > zeros) {
					hashVal++;
				}
				hashVal <<= 1;
				ones = zeros = 0;
				count--;
			}
		}
	}
	if (ones > zeros) {
		hashVal++;
	}
	ones = zeros = 0;
	count--;
	return hashVal;
}

void hashTable::insert(imgBlock* data) {
	int slot = this->hash(data);
	this->list[slot].pushBack(data);
}

imgBlock* hashTable::search(imgBlock* data) {
	int slot = this->hash(data);
	return this->list[slot].search(data);
}

int hashTable::bucketCount() {
	return this->bucketN;
}

int hashTable::bucketSize(int idx) {
	return this->list[idx].listSize();
}