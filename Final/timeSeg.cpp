#include "timeSeg.h"

timeSeg::timeSeg(double timeStr, double timeEnd){
    this->timeStr = timeStr;
    this->timeEnd = timeEnd;
}

timeSeg::~timeSeg(){
}

bool timeSeg::overlap(timeSeg t){
    return ((this->timeStr > t.timeEnd) || (this->timeEnd < t.timeStr)) ? false : true;
}

timeSeg timeSeg::operator+(const timeSeg &t){
    if(this->overlap(t)){
        double str, end;
        str = (this->timeStr < t.timeStr)? this->timeStr:t.timeStr;
        end = (this->timeEnd > t.timeEnd)? this->timeEnd:t.timeEnd;
        timeSeg result(str, end);
        return result;
    }else{
        return *this;
    }
}

timeSeg timeSeg::operator=(const timeSeg &t){
    this->timeStr=t.timeStr;
    this->timeEnd=t.timeEnd;
    return *this;
}

timeSeg timeSeg::operator+=(const timeSeg &t){
    *this = *this + t;
    return *this;
}

void timeSeg::print() {
    std::cout << "[ " << this->timeStr << " , " << this->timeEnd << " ]" << std::endl;
}

std::string timeSeg::getTimeSeg() {
    return "[ " + std::to_string(this->timeStr) + " , " + std::to_string(this->timeEnd) + " ]";
}
