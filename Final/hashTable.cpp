#include"hashTable.h"
hashTable::hashTable(int nBucket) {
	this->list = new singlyLinkedList[nBucket];
	this->bucketN = nBucket;
	this->bits = static_cast<int>(ceil(log2(nBucket)));
	this->segN = 50*50 / this->bits;
}

hashTable::hashTable(std::fstream& file) {
	std::string line;
	std::getline(file, line);
	this->bucketN = std::atoi(line.c_str());
	this->list = new singlyLinkedList[this->bucketN];
	this->bits = static_cast<int>(ceil(log2(this->bucketN)));
	this->segN = 50 * 50 / this->bits;
	while (std::getline(file, line)) {
		size_t pos = line.find('?');
		int bucketIdx;
		if (pos != std::string::npos) {
			bucketIdx = std::atoi(line.substr(0, pos).c_str());
			line = line.substr(pos, line.size());
		}
		int idx = 0;
		while(idx<line.size()) {
			if (line[idx] == '?') {
				cv::Mat keyM(50, 50, CV_8UC1);
				std::vector<timeSeg> time;
				idx++;
				for (int i = 0; i < keyM.rows; i++) {
					uchar* rows = keyM.ptr<uchar>(i);
					for (int j = 0; j < keyM.cols; j++) {
						rows[j] = (line[idx++] == '1') ? 255 : 0;
					}
				}
				if (line[idx] == '!') {
					idx++;
				}
				while (line[idx] != '?' && idx<line.size()){
					std::string timeS = "", timeE = "";
					while (line[idx] != '-') {
						timeS += line[idx];
						idx++;
					}
					idx++;
					while (line[idx] != ',') {
						timeE += line[idx];
						idx++;
					}
					time.push_back(timeSeg(std::atof(timeS.c_str()), std::atof(timeE.c_str())));
					idx++;
				}
				;
				this->list[bucketIdx].pushBack(imgBlock(keyM, time));
			}
		}
	}
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
	this->list[bucketIdx].pushBack(data);
}

imgBlock hashTable::search(imgBlock data) {
	int bucketIdx = this->hash(data);
	return this->list[bucketIdx].search(data);
}

int hashTable::bucketCount() {
	return this->bucketN;
}

int hashTable::bucketSize(int idx) {
	return this->list[idx].listSize();
}

void hashTable::save(std::string fileName) {
	std::fstream file;
	file.open("./videoHash/"+fileName+".hf", std::ios::out);
	if (!file) {
		std::cout << "[Error]" << std::endl;
	}
	else{
		file << this->bucketN << '\n';
		for (int i = 0; i < this->bucketN; i++) {
			if (this->list[i].listSize() != 0) {
				file << i;
				this->list[i].save(i, file);
				file << '\n';
			}
		}
	}
}
