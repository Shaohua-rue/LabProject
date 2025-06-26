#include <fstream>
#include <string>
#include <iostream>
#include "opencv2/opencv.hpp"
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;
int main()
{
    // 内参矩阵
    cv::Mat cameraMatrix1 = (cv::Mat_<double>(3, 3) << 400,0,512,
        0, 400, 512,
        0, 0, 1); // 第一个相机的内参矩阵

    cv::Mat cameraMatrix2 = (cv::Mat_<double>(3, 3) << 400, 0, 512,
        0, 400, 512,
        0, 0, 1); // 第二个相机的内参矩阵

    // 基础矩阵
    cv::Mat F1_2 = (cv::Mat_<double>(3, 3) << -2.0978e-06, - 1.45795e-07, - 0.00335113,
        - 4.28513e-06,  5.63787e-06,   0.00291568,
        0.00729516, - 0.00627653 ,     0.50552);

    // 图像点坐标
    // 将Point2d1和Point2d2分别填充到points1和points2中
    std::vector<cv::Point2d> points1, points2;
    Point2d x = { 822.428, 1133.53 };
    points1.push_back(x);
    Point2d y = { 808.709, 840.397 };
    points2.push_back(y);


    // 计算本质矩阵
    cv::Mat E1_2 = cameraMatrix2.t() * F1_2 * cameraMatrix1;

    // 计算相对姿态和平移向量
    cv::Mat relativeRotation, relativeTranslation;
    cv::recoverPose(E1_2, points1, points2, cameraMatrix2, relativeRotation, relativeTranslation);

    // 输出结果
    std::cout << "Relative Rotation Matrix R2:" << std::endl;
    std::cout << relativeRotation << std::endl;
    std::cout << "Relative Translation Vector t2:" << std::endl;
    std::cout << relativeTranslation << std::endl;

    return 0;
}
