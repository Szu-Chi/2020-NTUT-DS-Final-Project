#include"imgBlock.h"

imgBlock::imgBlock() {
}

imgBlock::imgBlock(cv::Mat frame, timeSeg t) {
    frame.copyTo(this->frame);
    this->time.push_back(t);
}

imgBlock::imgBlock(cv::Mat keyMat,std::vector<timeSeg> t) {
    keyMat.copyTo(this->keyMat);
    this->time = t;
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

void imgBlock::printTime() {
    
    for (int i = 0; i < this->time.size(); i++) {
        std::cout << "time segment #" << i << " "; 
        time[i].print();
    }
}
const timeSeg Max(2592000, 2592000);

//************************************************************************************
//************************************************************************************
//************************************************************************************
void Merge(std::vector<timeSeg>& Array, int front, int mid, int end) {

    // 利用 std::vector 的constructor, 
    // 把array[front]~array[mid]放進 LeftSub[]
    // 把array[mid+1]~array[end]放進 RightSub[]
    std::vector<timeSeg> LeftSub(Array.begin() + front, Array.begin() + mid + 1),
        RightSub(Array.begin() + mid + 1, Array.begin() + end + 1);

    LeftSub.insert(LeftSub.end(), Max);      // 在LeftSub[]尾端加入值為 Max 的元素
    RightSub.insert(RightSub.end(), Max);    // 在RightSub[]尾端加入值為 Max 的元素

    int idxLeft = 0, idxRight = 0;

    for (int i = front; i <= end; i++) {

        if (LeftSub[idxLeft].timeStr <= RightSub[idxRight].timeStr) {
            Array[i] = LeftSub[idxLeft];
            idxLeft++;
        }
        else {
            Array[i] = RightSub[idxRight];
            idxRight++;
        }
    }
}

void MergeSort(std::vector<timeSeg>& array, int front, int end) {
    // front與end為矩陣範圍
    if (front < end) {                   // 表示目前的矩陣範圍是有效的
        int mid = (front + end) / 2;         // mid即是將矩陣對半分的index
        MergeSort(array, front, mid);    // 繼續divide矩陣的前半段subarray
        MergeSort(array, mid + 1, end);    // 繼續divide矩陣的後半段subarray
        Merge(array, front, mid, end);   // 將兩個subarray做比較, 並合併出排序後的矩陣
    }
}
//************************************************************************************
//************************************************************************************
//************************************************************************************
//Reference: https://alrightchiu.github.io/SecondRound/comparison-sort-merge-sorthe-bing-pai-xu-fa.html
void imgBlock::save(std::string bucketSlot, std::fstream& file) {
    MergeSort(this->time, 0, this->time.size() - 1);
    for (int i = 1; i < this->time.size();) {
        if (this->time[i - 1].overlap(this->time[i])) {
            this->time[i - 1] += this->time[i];
        }
        else {
            i++;
        }
    }
    for (int i = 0; i < this->keyMat.rows; i++) {
        uchar* rows = this->keyMat.ptr<uchar>(i);
        for (int j = 0; j < this->keyMat.cols; j++) {
            if (rows[j] == 255)
                file << 1;
            else
                file << 0;
        }
    }
    file << '!';
    for (int i = 0; i < this->time.size(); i++) {
        file << this->time[i].getTimeSeg() << ',';
    }

    cv::imwrite("catchFrame/" + bucketSlot + ".bmp", this->frame);
    cv::imwrite("catchKeyMat/" + bucketSlot + ".bmp", this->keyMat);
    cv::imwrite("catchKeyMat/" + bucketSlot + "_.bmp", this->bfThersholdFrame);
    std::fstream file1;
    file1.open("timeSeg/" + bucketSlot + ".txt", std::ios::app| std::ios::out);
    if (!file1) {
        std::cout << "[error] ";
    }
    else {
        for (int i = 0; i < this->time.size(); i++) {
            std::string timeSeg = time[i].getTimeSeg();
            file1 << timeSeg << std::endl;
        }
    }
}