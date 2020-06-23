#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/xfeatures2d.hpp>

using namespace cv;
using namespace std;

class imgBlock {
private:
    double time;
public:
    imgBlock(Mat frame, double time);
    imgBlock();
    ~imgBlock();
    Mat frame;
    void save();
    void cp(imgBlock block);
};

imgBlock::imgBlock() {
}

imgBlock::imgBlock(Mat frame, double time) {
    frame.copyTo(this->frame);
    this->time = time;
}

imgBlock::~imgBlock() {

}

void imgBlock::save() {
    string frameFileName = to_string(time) + ".jpg";
    imwrite("seg/" + frameFileName, frame);
}

void imgBlock::cp(imgBlock block) {
    block.frame.copyTo(this->frame);
    this->time = block.time;
}


int main() {
    string videoFileName = "Alike.mp4";
    VideoCapture cap(videoFileName);
    if (!cap.isOpened()) {
        cout << "Error can't open " << videoFileName<< endl; 
        return -1;
    }
    namedWindow("Video", 1);
    Mat currFrame;
    double fps = cap.get(CV_CAP_PROP_FPS);

    int num = 0;
    queue<imgBlock> imgBlockQueue;
    cap >> currFrame;
    do
    {
        if (imgBlockQueue.size() < 8) {
            resize(currFrame, currFrame, Size(currFrame.cols/2,currFrame.rows/2));
            imgBlock block(currFrame, num++ / fps);
            imshow("currFrame", currFrame);
            imgBlockQueue.push(block);
            cap >> currFrame;
        }

        if (waitKey(25) >= 0) {
            while (!imgBlockQueue.empty()) {
                imgBlock temp;
                temp.cp(imgBlockQueue.front());
                temp.save();
                imgBlockQueue.pop();
            }
        }
    } while (!currFrame.empty());
    //system("PAUSE");
    return 0;
}