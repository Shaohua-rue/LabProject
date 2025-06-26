#ifndef ANALOG_CAMERA_H
#define ANALOG_CAMERA_H
#include <iostream>
#include <random>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
using namespace std;
class AnalogCamera
{
public:
    AnalogCamera();
    void Generate3DPoints();
    void Calculate2DPoints();
    void SolvePnP();
    void showCalibration();

    void GeneratePlanar3DPoints();
    void Calculate3DPoints();

     vector<double> real_x_w_;
    vector<double> real_y_w_;

    vector<double> calculate_x_w_;
    vector<double> calculate_y_w_;

private:
    cv::Mat K_;  //相机内参
    cv::Mat r_; //相机旋转向量
    cv::Mat R_;  //相机旋转矩阵
    cv::Mat t_;  //相机平移矩阵
    
    cv::Mat cal_r_;
    cv::Mat cal_t_;
    cv::Mat cal_R_;

    vector<cv::Point3f> objectPoints_;  //3D 点
    vector<cv::Point2f> imagePoints_;   //2D 点



    float z = 30;
};
#endif