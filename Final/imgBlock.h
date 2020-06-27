#ifndef IMGBLOCK_H
#define IMGBLOCK_H
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/opencv_modules.hpp>
#include "timeSeg.h"
class imgBlock {
private:
    std::vector<timeSeg> time;
    cv::Mat keyMat;
    cv::Mat bfThersholdFrame;
public:
    imgBlock(cv::Mat frame, timeSeg time);
    imgBlock(cv::Mat keyMat, std::vector<timeSeg> t);
    imgBlock();
    ~imgBlock();
    cv::Mat frame;
    void computeKeyMat();
    cv::Mat getKeyMat();
    void printTime();
    void save(std::string bucketSlot, std::fstream& file);
    static double computeMSE(cv::Mat image1, cv::Mat image2);
    imgBlock operator+(const imgBlock &block);
    imgBlock operator=(const imgBlock &block);
    imgBlock operator+=(const imgBlock &block);
    bool operator==(const imgBlock& block);
};
#endif // !IMGBLOCK_H
