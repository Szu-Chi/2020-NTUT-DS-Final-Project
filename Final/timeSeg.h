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
};

#endif // !TIMESEG_H
