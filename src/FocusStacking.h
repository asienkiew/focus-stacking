#pragma once    

#include <opencv2/opencv.hpp>
#include <iostream>

#include "Matrix8U.h"

class FocusStacking {
public:
    FocusStacking(std::string inDir, std::string outDir);

    FocusStacking(const FocusStacking& orig) = delete;
    virtual ~FocusStacking() = default;
    void generateResult();
    void writeResult();
private:
    std::string outDir;
    std::vector<cv::Mat> imageStackColor;
    std::vector<Matrix8U> imageStackGray;
    Matrix8U tmp;
    cv::Mat resultSharp;
    cv::Mat resultDepth;
    std::string getPathFromIndex(int i);


};
