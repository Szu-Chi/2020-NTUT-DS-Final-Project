#include"hashTable.h"
hashTable::hashTable(int nBucket) {
	this->list = new singlyLinkedList[nBucket];
	this->bucketN = nBucket;
	this->bits = static_cast<int>(ceil(log2(nBucket)));
	this->segN = 50*50 / this->bits;
}

hashTable::~hashTable() {
	delete[]this->list;
}

uint64_t hashTable::hash(imgBlock data) {
	uint64_t hashVal = 0;
	cv::Mat keyMat = data.getKeyMat();
	int zeros = 0;
	int ones = 0;
	int count = this->bits;
	int totalB = 0;
	for (int i = 0; i < keyMat.rows; i++) {
		for (int j = 0; j < keyMat.cols; j++) {
			uchar* pixel = keyMat.ptr<uchar>(i, j);
			if (int(*pixel) == 0) { 
				totalB++; 
			}
		}
	}

	for (int i = 0; i < keyMat.rows; i++) {
		for (int j = 0; j < keyMat.cols; j++) {
			uchar* pixel = keyMat.ptr<uchar>(i, j);
			(int(*pixel) == 255) ? ones++ : zeros++;
			if (ones + zeros == this->segN && count != 1) {
				if (zeros > totalB/this->bits) {
					hashVal++;
				}
				hashVal <<= 1;
				ones = zeros = 0;
				count--;
			}
		}
	}
	if (zeros > totalB / this->bits) {
		hashVal++;
	}
	ones = zeros = 0;
	count--;
	return hashVal;
}

void hashTable::insert(imgBlock data) {
	int bucketIdx = this->hash(data);
	this->list[bucketIdx].pushBack(data, bucketIdx);
}

imgBlock hashTable::search(imgBlock data) {
	int slot = this->hash(data);
	return this->list[slot].search(data);
}

int hashTable::bucketCount() {
	return this->bucketN;
}

int hashTable::bucketSize(int idx) {
	return this->list[idx].listSize();
}

void hashTable::print() {
	for (int i = 0; i < this->bucketN; i++) {
		std::cout << "Bucket #" << i << " have " << this->bucketSize(i) << " elements" << std::endl;
		this->list[i].print(i);
	}
}
