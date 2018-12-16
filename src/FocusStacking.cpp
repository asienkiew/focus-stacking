#include "FocusStacking.h"

#include <iomanip>
#include <sstream>

FocusStacking::FocusStacking(std::string inDir, std::string outDir) : outDir(outDir) {
    //reading could be done by multiple threads
    for (int i = 0; i < 13; i++) {
        imageStackColor.push_back(cv::imread(inDir + getPathFromIndex(i), CV_LOAD_IMAGE_COLOR));
        if (imageStackColor.back().empty()) {
            throw std::invalid_argument("Bad file:" + inDir + getPathFromIndex(i));
        }
        imageStackGray.emplace_back(imageStackColor.back());
    }
    //just create tmp,
    tmp = Matrix8U(imageStackColor[0].rows, imageStackColor[0].cols);
}

std::string FocusStacking::getPathFromIndex(int i) {
    std::stringstream fileName;
    fileName << "b_bigbug" << std::setfill('0') << std::setw(4) << i << "_croppped.png";
    return fileName.str();
}

void FocusStacking::generateResult() {
    /*
     * For every image:
     * 1) convert to gray scale (in constructor)
     * 2) blur with simple averaging
     * 3) apply laplacian operator to detect edges (edges are visible in sharp parts
     * 4) blur result
     */
    for (auto & m : imageStackGray) {
        Matrix8U::blur(m, tmp, 3);
        Matrix8U::laplacian(tmp, m);
        Matrix8U::blur(m, tmp, 7);
        tmp.copyTo(m);
    }

    resultSharp = cv::Mat(imageStackColor[0].rows, imageStackColor[0].cols, CV_8UC3);
    resultDepth = cv::Mat(imageStackColor[0].rows, imageStackColor[0].cols, CV_8UC1);

    /*
     * For every pixel use the sharpest pixel (measured by value of imageStackGray[i])
     *
     */
    for (int i = 0; i < resultSharp.rows; i++) {
        for (int j = 0; j < resultSharp.cols; j++) {

            size_t best = 0; //best photo for given pixel
            uchar maxVal = 0;
            for (size_t im = 0; im < imageStackGray.size(); im++) {
                if (imageStackGray[im].at(i, j) > maxVal) {
                    maxVal = imageStackGray[im].at(i, j);
                    best = im;
                }
            }
            resultSharp.at<cv::Vec3b>(i, j) = imageStackColor[best].at<cv::Vec3b>(i, j);
            const int THRESHOLD = 8;
            if (maxVal < THRESHOLD) {
                //it is noise probably
                resultDepth.at<uchar>(i, j) = maxVal;
            } else {
                // map index of photo to value of depth
                //to use more than 13 values some weighted average could be used
                //instead of taking max
                resultDepth.at<uchar>(i, j) = THRESHOLD +
                        (255 - THRESHOLD) * (1 - (float) best / imageStackGray.size());
            }
        }
    }
}

void FocusStacking::writeResult() {
    std::vector<int> compressionParams{CV_IMWRITE_PNG_COMPRESSION, 9};
    cv::imwrite(outDir + "resultSharp.png", resultSharp, compressionParams);
    cv::imwrite(outDir + "resultDepth.png", resultDepth, compressionParams);
}
