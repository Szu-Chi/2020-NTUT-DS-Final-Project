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
    if (this->frame.empty()) {
        return;
    }
    cv::cuda::GpuMat mSource(this->frame);
    cv::cuda::GpuMat mGrayscale, mResize250, mResize50, mEdgeEnhance, mGaussBlur;

    cv::cuda::cvtColor(mSource, mGrayscale, cv::COLOR_RGB2GRAY, CV_8UC1);          //RGB -> Gray

    cv::Ptr<cv::cuda::Filter> gaussianFilter = cv::cuda::createGaussianFilter(mGrayscale.type(), mGaussBlur.type(), cv::Size(3, 3), 5, 5);
    gaussianFilter->apply(mGrayscale, mGaussBlur);     //Gaussin filter
    
    cv::cuda::resize(mGaussBlur, mResize250, cv::Size(250, 250));             //Resize

    cv::cuda::GpuMat cannyFrame;
    cv::Ptr<cv::cuda::CannyEdgeDetector> canny = cv::cuda::createCannyEdgeDetector(10, 50);
    canny->detect(mResize250, cannyFrame);                            //Edge detection
    cv::cuda::addWeighted(mResize250, 0.7, cannyFrame, 0.3, 0, mEdgeEnhance);
   
    gaussianFilter->apply(mEdgeEnhance, mGaussBlur);     //Gaussin filter
    cv::cuda::resize(mGaussBlur, mResize50, cv::Size(50, 50));               //Resize
    mGaussBlur.download(this->bfThersholdFrame);

    mResize50.download(this->keyMat);
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

bool imgBlock::operator==(const imgBlock& block) {
    imgBlock self = *this;
    return self.time[0] == block.time[0];
}

double imgBlock::computeMSE(cv::Mat image1, cv::Mat image2) {
    cv::cuda::GpuMat mImg1(image1);
    cv::cuda::GpuMat mImg2(image2);
    cv::cuda::GpuMat mDiff, mDiff_sqr;
    cv::cuda::absdiff(mImg1, mImg2, mDiff);
    cv::cuda::sqr(mDiff, mDiff_sqr);
    cv::Scalar s = cv::cuda::sum(mDiff_sqr);
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

    // �Q�� std::vector ��constructor, 
    // ��array[front]~array[mid]��i LeftSub[]
    // ��array[mid+1]~array[end]��i RightSub[]
    std::vector<timeSeg> LeftSub(Array.begin() + front, Array.begin() + mid + 1),
        RightSub(Array.begin() + mid + 1, Array.begin() + end + 1);

    LeftSub.insert(LeftSub.end(), Max);      // �bLeftSub[]���ݥ[�J�Ȭ� Max ������
    RightSub.insert(RightSub.end(), Max);    // �bRightSub[]���ݥ[�J�Ȭ� Max ������

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
    // front�Pend���x�}�d��
    if (front < end) {                   // ��ܥثe���x�}�d��O���Ī�
        int mid = (front + end) / 2;         // mid�Y�O�N�x�}��b����index
        MergeSort(array, front, mid);    // �~��divide�x�}���e�b�qsubarray
        MergeSort(array, mid + 1, end);    // �~��divide�x�}����b�qsubarray
        Merge(array, front, mid, end);   // �N���subarray�����, �æX�֥X�Ƨǫ᪺�x�}
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
            this->time.erase(this->time.begin() + i);
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