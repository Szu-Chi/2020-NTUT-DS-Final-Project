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
#include<Windows.h>
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
	string videoFileName = "DSclass";
	string imagePath = "./inputImg/2445-0.bmp";
	//User
	cout << "Please enter video file name: ";
	cin >> videoFileName;
	cout << "Please enter image (absolute path): ";
	cin >> imagePath;
	cout << '\n';

	Mat img = imread(imagePath);

	if (img.empty()) {
		size_t pos = imagePath.find_last_of("/");
		cout << "[Fail] Input image isn't existing. Please check it." << endl;
		cout << "Input image file: " << imagePath.substr(pos + 1, imagePath.size()) << endl;
		cout << "Input path: " << imagePath.substr(0, pos + 1) << endl;
	}
	else {
		
		LARGE_INTEGER freq;
		LARGE_INTEGER beginTime;
		LARGE_INTEGER endTime;

		string videoExtension = "mp4";
		fstream file;
		file.open("./videoHash/" + videoFileName + ".ht", ios::in);

		if (file) {
			hashTable hashTab(file);
			
			std::cout << "Searching..." << std::endl;
			QueryPerformanceFrequency(&freq);
			QueryPerformanceCounter(&beginTime);
			//---tic---
			imgBlock null = imgBlock(cv::Mat(1, 1, 0), timeSeg(-1, -1));
			imgBlock imgB(img, timeSeg(-1, -1));
			imgB.computeKeyMat();
			imgBlock res = hashTab.search(imgB);
			//---toc---
			QueryPerformanceCounter(&endTime);
			cout << "Search completed." << std::endl;
			cout << "Search time: " << static_cast<double>((endTime.QuadPart - beginTime.QuadPart)) / freq.QuadPart << "s\n" << endl;
			cout << "[Search Result]" << endl;
			if (res == null) {
				cout << "Fail! Based on the input image, it's unable to find the corresponding time segment." << endl;
			}
			else {
				res.printTime();
			}
		}
		else {			
			VideoCapture cap(videoFileName + "." + videoExtension);
			std::cout << "\nHash table file of the video isn't existing." << std::endl;
			if (!cap.isOpened()) {
				cout << "[Fail] There are no video file name called " << videoFileName << "." << endl;
			}
			else {
				std::cout << "Hash table building..." << std::endl;

				QueryPerformanceFrequency(&freq);
				QueryPerformanceCounter(&beginTime);
				//---tic---
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
				//---toc---
				QueryPerformanceCounter(&endTime);
				cout << "Build completed." << std::endl;
				cout << "Build time: " << static_cast<double>((endTime.QuadPart - beginTime.QuadPart)) / freq.QuadPart << " s\n" << endl;

				std::cout << "Searching..." << std::endl;
				QueryPerformanceFrequency(&freq);
				QueryPerformanceCounter(&beginTime);
				//---tic---
				imgBlock null = imgBlock(cv::Mat(1, 1, 0), timeSeg(-1, -1));
				imgBlock imgB(img, timeSeg(-1, -1));
				imgB.computeKeyMat();
				imgBlock res = hashTab.search(imgB);
				//---toc---
				QueryPerformanceCounter(&endTime);
				cout << "Search completed." << std::endl;
				cout << "Search time: " << static_cast<double>((endTime.QuadPart - beginTime.QuadPart)) / freq.QuadPart << " s\n" << endl;
				
				cout << "[Search Result]" << endl;
				if (res == null) {
					cout << "Sorry! Based on the input image, it's unable to find the corresponding time segment." << endl;
				}
				else {
					res.printTime();
				}
			}
		}
	}
	cout << '\n';
	system("pause");
	return 0;
}