#include"imgBlock.h"

imgBlock::imgBlock() {
}

imgBlock::imgBlock(cv::Mat frame, timeSeg t) {
    frame.copyTo(this->frame);
    this->time.push_back(t);
}

imgBlock::~imgBlock() {

}

void imgBlock::cp(imgBlock block) {
    block.frame.copyTo(this->frame);
    this->time = block.time;
    this->keyMat = block.keyMat;
}

void imgBlock::computeKeyMat() {
    cv::cvtColor(this->frame, this->keyMat, CV_RGB2GRAY, CV_8UC1);      //RGB -> Gray
    cv::GaussianBlur(this->keyMat, this->keyMat, cv::Size(9, 9), 10, 10);   //Gaussin filter
    cv::resize(this->keyMat, this->keyMat, cv::Size(8, 8));                 //Resize
    cv::Canny(this->keyMat, this->keyMat, 10, 50);                      //Edge detection
    cv::threshold(this->keyMat, this->keyMat, 0, 255, cv::THRESH_OTSU);     //Binarization, OTSU algo.
}

cv::Mat imgBlock::getKeyMat() {
    return this->keyMat;
}

void imgBlock::setKeyMat(cv::Mat mat) {
    this->keyMat = mat;
}

imgBlock imgBlock::operator+(const imgBlock &block){
    imgBlock result, temp;
    result = *this;
    temp = block;
    while(!temp.time.empty()){
        timeSeg t = temp.time.front();
        temp.time.pop_back();
        for(int i = 0; i < result.time.size(); i++){
            if(result.time.at(i).overlap(t)){
                result.time.at(i) += t;
                break;
            }
        }
        result.time.push_back(t);
    }    
    return result;
}

imgBlock imgBlock::operator=(const imgBlock &block){
    block.frame.copyTo(this->frame);
    this->time = block.time;
    this->keyMat = block.keyMat;
    return *this;
}

imgBlock imgBlock::operator+=(const imgBlock &block){
    *this = *this + block;
    return *this;
}

double imgBlock::computeMSE(cv::Mat image1, cv::Mat image2) {
    cv::Mat difference;
    absdiff(image1, image2, difference);
    difference = difference.mul(difference);
    cv::Scalar s = sum(difference);
    double sse = s.val[0];
    return sse / double(image1.total());
}