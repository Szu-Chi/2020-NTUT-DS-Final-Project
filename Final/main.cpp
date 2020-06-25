#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/xfeatures2d.hpp>
#include<queue>
#include"hashTable.h"

using namespace cv;
using namespace std;

int main() {
    //string videoFileName = "Alike.mp4";
    string videoFileName = "DSclass_1.mp4";

    VideoCapture cap(videoFileName);
    if (!cap.isOpened()) {
        cout << "Error can't open " << videoFileName<< endl; 
        return -1;
    }
    namedWindow("Video", 1);
    Mat currFrame;
    double fps = cap.get(CV_CAP_PROP_FPS);
    double nFrame = cap.get(CV_CAP_PROP_FRAME_COUNT);
    int num = 0;
    queue<imgBlock> imgBlockQueue;
    hashTable hashTab(static_cast<int>(pow(2, ceil(log2(ceil(nFrame / 20))))));
    cap >> currFrame;
    resize(currFrame, currFrame, Size(currFrame.cols / 2, currFrame.rows / 2));
    imgBlock ref(currFrame, timeSeg(0, 0));
    ref.computeKeyMat();

    imgBlock cur;
    double threshold = 70;
    do
    {
        if (imgBlockQueue.size() < 8) {
            imgBlock block(currFrame, timeSeg(num / fps, (num+1) / fps));
            num++;
            imgBlockQueue.push(block);
            cap >> currFrame;
            if (currFrame.empty()) {
                break;
            }
            resize(currFrame, currFrame, Size(currFrame.cols / 2, currFrame.rows / 2));
        }

        while (!imgBlockQueue.empty()) {
            cur = imgBlockQueue.front();
            imgBlockQueue.pop();
            cur.computeKeyMat();
            hashTab.insert(cur);
        }
    } while (!currFrame.empty());
    hashTab.print();
    ;
    return 0;
}