#include "Matrix8U.h"

#include <cassert>

constexpr int Matrix8U::LAPLACIAN[3][3];

Matrix8U::Matrix8U() : Matrix8U(0, 0) {

}

Matrix8U::Matrix8U(int rows, int cols) : data(rows * cols, 0), rows(rows), cols(cols) {

}

Matrix8U::Matrix8U(cv::Mat mat) : Matrix8U(mat.rows, mat.cols) {
    //accepts only 8UC3 format
    assert(mat.type() == CV_8UC3);
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            //convert to gray
            data[ cols * i + j] = 0.25 * mat.at<cv::Vec3b>(i, j)[0] +
                    0.5 * mat.at<cv::Vec3b>(i, j)[1] +
                    0.25 * mat.at<cv::Vec3b>(i, j)[2];
        }
    }
}

void Matrix8U::copyTo(Matrix8U& dst) {
    dst.data = data;
}

void Matrix8U::blur(const Matrix8U& in, Matrix8U& out, int size) {
    if (size % 2 == 0) {
        throw std::invalid_argument("Size should be even");
    }
    if (in.rows != out.rows || in.cols != out.cols) {
        throw std::invalid_argument("Size should be even");
    }
    //use simple averaging for simplicity
    for (int i = 0; i < in.rows; i++) {
        for (int j = 0; j < in.cols; j++) {
            int acc = 0;
            for (int ki = -(size / 2); ki <= (size / 2); ki++) { //kernel i
                for (int kj = -(size / 2); kj <= (size / 2); kj++) { //kernel j
                    //limit indices to image area

                    int iLimit = std::min(std::max(i + ki, 0), in.rows - 1);
                    int jLimit = std::min(std::max(j + kj, 0), in.cols - 1);
                    acc += in.data[in.cols * iLimit + jLimit];
                }
            }
            out.data[ in.cols * i + j] = acc / (size * size);

        }
    }
}

void Matrix8U::laplacian(const Matrix8U& in, Matrix8U& out) {
    for (int i = 0; i < in.rows; i++) {
        for (int j = 0; j < in.cols; j++) {
            int acc = 0;
            for (int ki = -1; ki <= 1; ki++) { //kernel i
                for (int kj = -1; kj <= 1; kj++) { //kernel j
                    //limit indices to image area

                    int iLimit = std::min(std::max(i + ki, 0), in.rows - 1);
                    int jLimit = std::min(std::max(j + kj, 0), in.cols - 1);
                    acc += LAPLACIAN[ki + 1][kj + 1] * in.data[in.cols * iLimit + jLimit];
                }
            }
            //limit values to range of uint8_t
            //maybe using float would increase accuracy?
            out.data[ in.cols * i + j] = std::min(std::max(acc, 0), 255);

        }
    }
}

unsigned char& Matrix8U::at(int i, int j) {
    return data[ cols * i + j];
}
