#include "timeSeg.h"

timeSeg::timeSeg(double timeStr, double timeEnd){
    this->timeStr = timeStr;
    this->timeEnd = timeEnd;
}

timeSeg::~timeSeg(){
}

bool timeSeg::overlap(timeSeg t){
    return ((this->timeStr > t.timeEnd + 5) || (this->timeEnd < t.timeStr - 5)) ? false : true;
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

bool timeSeg::operator==(const timeSeg& t) {
    timeSeg self = *this;
    return self.timeStr == t.timeStr && self.timeEnd == t.timeEnd;
}

void timeSeg::print() {
    int hour;
    int min;
    int sec;
    int milliSec;
    std::string start, end;
    hour = this->timeStr / 3600;
    min = this->timeStr / 60;
    sec = this->timeStr - hour * 3600 - min * 60;
    milliSec = std::ceil((this->timeStr - std::floor(this->timeStr)) * 1000);
    start = std::to_string(hour) + ":" + std::to_string(min) + ":" + std::to_string(sec);// +":" + std::to_string(milliSec);
    
    hour = this->timeEnd / 3600;
    min = this->timeEnd / 60;
    sec = this->timeEnd - hour * 3600 - min * 60;
    milliSec = std::ceil((this->timeEnd - std::floor(this->timeEnd)) * 1000);
    end = std::to_string(hour) + ":" + std::to_string(min) + ":" + std::to_string(sec);// +":" + std::to_string(milliSec);
    std::cout << "[" + start + " ~ " + end + "]" << std::endl;
}

std::string timeSeg::getTimeSeg() {
    return std::to_string(this->timeStr) + "-" + std::to_string(this->timeEnd);
}

