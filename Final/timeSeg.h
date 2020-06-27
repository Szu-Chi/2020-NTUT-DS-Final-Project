#ifndef TIMESEG_H
#define TIMESEG_H
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
class timeSeg{
private:
    double timeStr;
    double timeEnd;
public:
    timeSeg();
    timeSeg(double timeStr, double timeEnd);
    ~timeSeg();
    bool overlap(timeSeg t);
    timeSeg operator+(const timeSeg &t);
    timeSeg operator=(const timeSeg &t);
    timeSeg operator+=(const timeSeg &t);
    void print();
    std::string getTimeSeg();
    friend void Merge(std::vector<timeSeg>& Array, int front, int mid, int end);
};
#endif // !TIMESEG_H
