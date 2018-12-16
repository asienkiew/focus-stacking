#include "FocusStacking.h"

#include <iomanip>
#include <sstream>

constexpr int FocusStacking::LAPLACIAN[3][3];

std::string type2str(int type) {
    std::string r;

    uchar depth = type & CV_MAT_DEPTH_MASK;
    uchar chans = 1 + (type >> CV_CN_SHIFT);

    switch (depth) {
        case CV_8U: r = "8U";
            break;
        case CV_8S: r = "8S";
            break;
        case CV_16U: r = "16U";
            break;
        case CV_16S: r = "16S";
            break;
        case CV_32S: r = "32S";
            break;
        case CV_32F: r = "32F";
            break;
        case CV_64F: r = "64F";
            break;
        default: r = "User";
            break;
    }

    r += "C";
    r += (chans + '0');

    return r;
}

FocusStacking::FocusStacking(std::string inDir, std::string outDir) : outDir(outDir) {
    for (int i = 0; i < 13; i++) {
        imageStackColor.push_back(cv::imread(inDir + getPathFromIndex(i), CV_LOAD_IMAGE_COLOR));

        if (imageStackColor.back().empty()) {
            throw std::invalid_argument("Bad file:" + inDir + getPathFromIndex(i));
        }
        imageStackGray.push_back(cv::imread(inDir + getPathFromIndex(i), CV_LOAD_IMAGE_GRAYSCALE));
    }
    std::cout << type2str(imageStackColor[0].type()) << " " << type2str(imageStackGray[0].type());
}

std::string FocusStacking::getPathFromIndex(int i) {
    std::stringstream fileName;
    fileName << "b_bigbug" << std::setfill('0') << std::setw(4) << i << "_croppped.png";
    return fileName.str();
}

void FocusStacking::generateResult() {
    for (auto & m : imageStackGray) {
        cv::blur(m, tmp, cv::Size(3, 3));
        cv::Laplacian(tmp, m, CV_8U, 1, 1);
        cv::blur(m, tmp, cv::Size(7, 7));
        tmp.copyTo(m);
    }
    imageStackColor[0].copyTo(resultSharp);
    imageStackGray[0].copyTo(resultDepth);
    for (int i = 0; i < resultSharp.rows; i++) {
        for (int j = 0; j < resultSharp.cols; j++) {
            size_t best = 0; //best photo for given pixel
            uchar maxVal = 0;
            for (size_t im = 0; im < imageStackGray.size(); im++) {
                if (imageStackGray[im].at<uchar>(i, j) > maxVal) {
                    maxVal = imageStackGray[im].at<uchar>(i, j);
                    best = im;
                }
            }
            resultSharp.at<cv::Vec3b>(i, j) = imageStackColor[best].at<cv::Vec3b>(i, j);
            const int THRESHOLD = 3;
            if (maxVal < THRESHOLD) {
                resultDepth.at<uchar>(i, j) = maxVal;
            } else {
                resultDepth.at<uchar>(i, j) = THRESHOLD + (255 - THRESHOLD) * (1 - (float) best / imageStackGray.size());
            }
        }
    }

}

void FocusStacking::writeResult() {
    std::vector<int> compressionParams{CV_IMWRITE_PNG_COMPRESSION, 9};
    int i = 0;
    for (auto & m : imageStackGray) {
        cv::imwrite(outDir + "result" + std::to_string(i++) + ".png", m, compressionParams);
    }
    cv::imwrite(outDir + "resultSharp.png", resultSharp, compressionParams);
    cv::imwrite(outDir + "resultDepth.png", resultDepth, compressionParams);
}
