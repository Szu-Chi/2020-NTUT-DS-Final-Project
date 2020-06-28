#ifndef HASHTABLE_H
#define HASHTABLE_H
#include"singlyLinkedList.h"
extern std::mutex mu;
class hashTable {
private:
	singlyLinkedList* list;
	int bucketN;
	int segN;
	int bits;
public:
	hashTable(int space);
	hashTable(std::fstream& file);
	~hashTable();
	uint64_t hash(imgBlock data);
	void insert(imgBlock data);
	imgBlock search(imgBlock data);
	int bucketCount();
	int bucketSize(int idx);
	void save(std::string fileName);
};
#endif // !HASHTABLE_H

