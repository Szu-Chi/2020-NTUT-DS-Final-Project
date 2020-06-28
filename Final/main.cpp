#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/xfeatures2d.hpp>
#include<queue>
#include"hashTable.h"
#include<thread>
#include<mutex>

using namespace cv;
using namespace std;
std::mutex mu;
bool finish = false;
const auto processor_count = std::thread::hardware_concurrency();
void readFrame(queue<imgBlock>& imgBlockQueue, VideoCapture& cap) {
	Mat currFrame;
	double fps = cap.get(CAP_PROP_FPS);
	int num = 0;
	do {
		if (mu.try_lock()) {
			if (imgBlockQueue.size() < processor_count*2) {
				cap >> currFrame;
				if (currFrame.empty()) {
					mu.unlock();
					break;
				}
				imgBlock block(currFrame, timeSeg(num / fps, (num + 1) / fps));
				num++;
				imgBlockQueue.push(block);
			}
			mu.unlock();
		}
	} while (!currFrame.empty());
	finish = true;
}

void frameProc(queue<imgBlock>& imgBlockQueue, hashTable& hashTab) {
	while (finish == false) {
		imgBlock cur;
		if (mu.try_lock()) {
			if (imgBlockQueue.empty()) {
				mu.unlock();
			}
			else {
				cur = imgBlockQueue.front();
				imgBlockQueue.pop();
				mu.unlock();
				cur.computeKeyMat();
				hashTab.insert(cur);
			}
		}
	}
}

int main(int argc, char* argv[]) {
	//User
	string videoFileName = "DSclass_1";
	Mat img= imread("./inputImg/357-0.bmp");
	if (img.empty()) {
		cout << "Input image isn't existing. Please check it." << endl;
	}
	else {
		imgBlock imgB(img, timeSeg(-1, -1));
		imgB.computeKeyMat();
		string videoExtension = "mp4";
		fstream file;
		file.open("./videoHash/" + videoFileName + ".hf", ios::in);
		if (file) {
			hashTable hashTab(file);
			imgBlock null = imgBlock(cv::Mat(1, 1, 0), timeSeg(-1, -1));
			imgBlock res = hashTab.search(imgB);
			cout << "[Search Result]" << endl;
			if (res == null) {
				cout << "Fail! Based on the input image, it's unable to find the corresponding time segment." << endl;
			}
			else {
				res.printTime();
			}
		}
		else {
			std::cout << "Hash table of the video isn't existing." << std::endl;
			VideoCapture cap(videoFileName + "." + videoExtension);
			if (!cap.isOpened()) {
				cout << "[Error] There are no video file name called " << videoFileName << "." << endl;
			}
			else {
				std::cout << "Hash table constructing.." << std::endl;
				queue<imgBlock> imgBlockQueue;
				double nFrame = cap.get(CAP_PROP_FRAME_COUNT);
				hashTable hashTab(static_cast<int>(pow(2, ceil(log2(ceil(nFrame / 20))))));
				thread reader{ readFrame, ref(imgBlockQueue), ref(cap) };
				vector<thread> frameProcThreads;
				for (int i = 0; i < processor_count; i++) {
					frameProcThreads.push_back(thread{ frameProc, ref(imgBlockQueue), ref(hashTab) });
				}
				reader.join();
				for (int i = 0; i < frameProcThreads.size(); i++) {
					frameProcThreads[i].join();
				}
				hashTab.save(videoFileName);
				imgBlock null = imgBlock(cv::Mat(1, 1, 0), timeSeg(-1, -1));
				imgBlock res = hashTab.search(imgB);
				cout << "[Search Result]" << endl;
				if (res == null) {
					cout << "Fail! Based on the input image, it's unable to find the corresponding time segment." << endl;
				}
				else {
					res.printTime();
				}
			}
		}
	}
	system("pause");
	return 0;
}