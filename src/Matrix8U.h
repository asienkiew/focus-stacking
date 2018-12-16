#pragma once

#include <opencv2/highgui/highgui.hpp>

/* 
 * 2D matrix of unsigned char
 * mimics OpenCV Mat
 */
class Matrix8U {
public:
    Matrix8U();
    //don't take into account stride for simplicity
    Matrix8U(int rows, int cols);
    Matrix8U(cv::Mat mat);

    virtual ~Matrix8U() = default;
    unsigned char & at(int i, int j);
    void copyTo(Matrix8U & dst);
    static void blur(const Matrix8U & in, Matrix8U & out, int size);
    static void laplacian(const Matrix8U & in, Matrix8U & out);
private:
    std::vector<unsigned char> data;
    int rows;
    int cols;
    static constexpr int LAPLACIAN[3][3] = {
        {-1, -1, -1},
         {-1, 8, -1},
         {-1, -1, -1}
    };

};
