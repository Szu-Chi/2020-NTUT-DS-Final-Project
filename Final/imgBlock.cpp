#include"imgBlock.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/xfeatures2d.hpp>
using namespace cv;
using namespace std;

imgBlock::imgBlock() {
}

imgBlock::imgBlock(Mat frame, double time) {
    frame.copyTo(this->frame);
    this->time = time;
}

imgBlock::~imgBlock() {

}

void imgBlock::cp(imgBlock block) {
    block.frame.copyTo(this->frame);
    this->time = block.time;
    this->keyMat = block.keyMat;
}

void imgBlock::computeKeyMat() {
    cvtColor(this->frame, this->keyMat, CV_RGB2GRAY, CV_8UC1);      //RGB -> Gray
    GaussianBlur(this->keyMat, this->keyMat, Size(9, 9), 10, 10);   //Gaussin filter
    resize(this->keyMat, this->keyMat, Size(8, 8));                 //Resize
    Canny(this->keyMat, this->keyMat, 10, 50);                      //Edge detection
    threshold(this->keyMat, this->keyMat, 0, 255, THRESH_OTSU);     //Binarization, OTSU algo.
    Mat show;
    keyMat.copyTo(show);
}

Mat imgBlock::getKeyMat() {
    return this->keyMat;
}

void imgBlock::setKeyMat(Mat mat) {
    this->keyMat = mat;
}

/*
void imgBlock::operator=(imgBlock block) {
    block.frame.copyTo(this->frame);
    this->time = block.time;
    this->keyMat = block.keyMat;
}*/

double imgBlock::computeMSE(Mat image1, Mat image2) {
    Mat difference;
    absdiff(image1, image2, difference);
    difference = difference.mul(difference);
    Scalar s = sum(difference);
    double sse = s.val[0];
    return sse / double(image1.total());
}