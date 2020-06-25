#ifndef IMGBLOCK_H
#define IMGBLOCK_H
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;
class imgBlock {
private:
    double time;
    Mat keyMat;
public:
    imgBlock(Mat frame, double time);
    imgBlock();
    ~imgBlock();
    Mat frame;
    void cp(imgBlock block);
    void computeKeyMat();
    Mat getKeyMat();
    void setKeyMat(Mat mat);
    static double computeMSE(Mat image1, Mat image2);
};
#endif // !IMGBLOCK_H
