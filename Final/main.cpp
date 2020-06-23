#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/xfeatures2d.hpp>
#include<queue>

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
    Mat key;
    void save();
    void cp(imgBlock block);
    void computeKeyMat();
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
    this->key = block.key;
}

void imgBlock::computeKeyMat() {
    cvtColor(this->frame, this->key, CV_RGB2GRAY, CV_8UC1);
    GaussianBlur(this->key, this->key, Size(9, 9), 10, 10);
    resize(this->key, this->key, Size(8, 8));
    Canny(this->key, this->key, 10, 50);
    threshold(this->key, this->key, 0, 255, THRESH_OTSU);
    Mat show;
    key.copyTo(show);
}

void PRINT(Mat iamge) {
    for (int i = 0; i < iamge.size().width; i++) {
        for (int j = 0; j < iamge.size().height; j++) {
            uchar* pixel = iamge.ptr<uchar>(i, j);
            std::cout << int(*pixel) << " ";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

double computeMSE(Mat image1, Mat image2) {
    Mat difference;
    absdiff(image1, image2, difference);
    difference = difference.mul(difference);
    Scalar s = sum(difference);
    double sse = s.val[0];
    return sse / double(image1.total());
}

int main() {
    string videoFileName = "DSclass.mp4";
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
    resize(currFrame, currFrame, Size(currFrame.cols / 2, currFrame.rows / 2));
    imgBlock ref(currFrame, 0);
    ref.computeKeyMat();
    imgBlock cur;
    double threshold = 70;
    do
    {
        if (imgBlockQueue.size() < 8) {
            imgBlock block(currFrame, num++ / fps);
            imgBlockQueue.push(block);
            cap >> currFrame;
            if (!currFrame.empty()) {
                break;
            }
            resize(currFrame, currFrame, Size(currFrame.cols / 2, currFrame.rows / 2));
        }

        while (!imgBlockQueue.empty()) {
            cur.cp(imgBlockQueue.front());
            imgBlockQueue.pop();
            cur.computeKeyMat();
            double mse = computeMSE(ref.key, cur.key);
            if (mse > threshold) {
                ref.cp(cur);
            }
        }
    } while (!currFrame.empty());
    return 0;
}