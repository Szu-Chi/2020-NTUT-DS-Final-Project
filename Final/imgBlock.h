#ifndef IMGBLOCK_H
#define IMGBLOCK_H
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <opencv2/opencv_modules.hpp>
#include <opencv2/xfeatures2d.hpp>
#include "timeSeg.h"

class imgBlock {
private:
    std::vector<timeSeg> time;
    cv::Mat keyMat;
public:
    imgBlock(cv::Mat frame, timeSeg time);
    imgBlock();
    ~imgBlock();
    cv::Mat frame;
    void cp(imgBlock block);
    void computeKeyMat();
    cv::Mat getKeyMat();
    void setKeyMat(cv::Mat mat);
    static double computeMSE(cv::Mat image1, cv::Mat image2);
    imgBlock operator+(const imgBlock &block);
    imgBlock operator=(const imgBlock &block);
    imgBlock operator+=(const imgBlock &block);
};
#endif // !IMGBLOCK_H
