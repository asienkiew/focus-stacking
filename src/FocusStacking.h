#pragma once    

#include <opencv2/opencv.hpp>
#include <iostream>

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
    std::vector<cv::Mat> imageStackGray;
    cv::Mat tmp;
    cv::Mat resultSharp;
    cv::Mat resultDepth;
    std::string getPathFromIndex(int i);
    static constexpr int LAPLACIAN[3][3] = {
    {0, 1, 0},
    {1, 4, 1},
    {0, 1, 0}};

};
