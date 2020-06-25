#include"imgBlock.h"

imgBlock::imgBlock() {
}

imgBlock::imgBlock(cv::Mat frame, timeSeg t) {
    frame.copyTo(this->frame);
    this->time.push_back(t);
}

imgBlock::~imgBlock() {

}

void imgBlock::computeKeyMat() {
    cv::cvtColor(this->frame, this->keyMat, CV_RGB2GRAY, CV_8UC1);      //RGB -> Gray
    cv::resize(this->keyMat, this->keyMat, cv::Size(300, 300));                 //Resize
    cv::Mat cannyFrame;
    cv::GaussianBlur(this->keyMat, this->keyMat, cv::Size(9, 9), 10, 10);   //Gaussin filter
    cv::equalizeHist(this->keyMat, this->keyMat);
    cv::Canny(this->keyMat, cannyFrame, 10, 50);                      //Edge detection
    cv::addWeighted(this->keyMat,0.7, cannyFrame,0.3, 0,this->keyMat);
    cv::GaussianBlur(this->keyMat, this->keyMat, cv::Size(3, 3), 5, 5);   //Gaussin filter
    keyMat.copyTo(bfThersholdFrame);
    cv::resize(this->keyMat, this->keyMat, cv::Size(50, 50));                 //Resize
    cv::threshold(this->keyMat, this->keyMat, 0, 255, cv::THRESH_OTSU);     //Binarization, OTSU algo.
}

cv::Mat imgBlock::getKeyMat() {
    return this->keyMat;
}

imgBlock imgBlock::operator+(const imgBlock &block){
    imgBlock result, temp;
    result = *this;
    temp = block;
    while(!temp.time.empty()){
        timeSeg t = temp.time.front();
        temp.time.pop_back();
        bool timeMerge = false;
        for(int i = 0; i < result.time.size(); i++){
            if(result.time.at(i).overlap(t)){
                result.time.at(i) += t;
                timeMerge = true;
                break;
            }
        }
        if (!timeMerge) {
            result.time.push_back(t);
        }
    }    
    return result;
}

imgBlock imgBlock::operator=(const imgBlock &block){
    block.frame.copyTo(this->frame);
    this->time = block.time;
    block.keyMat.copyTo(this->keyMat);
    block.bfThersholdFrame.copyTo(this->bfThersholdFrame);
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

void imgBlock::print() {
    std::cout << "time segment" << std::endl;
    for (int i = 0; i < this->time.size(); i++) {
        time[i].print();
    }
}

void imgBlock::save(std::string bucketSlot) {
    cv::imwrite("catchFrame/" + bucketSlot + ".bmp", this->frame);
    cv::imwrite("catchKeyMat/" + bucketSlot + ".bmp", this->keyMat);
    cv::imwrite("catchKeyMat/" + bucketSlot + "_.bmp", this->bfThersholdFrame);
    std::fstream file;
    file.open("timeSeg/" + bucketSlot + ".txt", std::ios::app| std::ios::out);
    if (!file) {
        std::cout << "[error] ";
    }
    else {
        for (int i = 0; i < this->time.size(); i++) {
            std::string timeSeg = time[i].getTimeSeg();
            file << timeSeg << std::endl;
            std::cout << timeSeg << std::endl;
        }
    }
}