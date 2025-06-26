#include <iostream>
#include <vector>
#include <random>
#include <fstream>
#include <opencv2/opencv.hpp>
#include "Read3dPoints.h"

int main()
{
    // 相机内参
    double fx = 400.0;  // 焦距（x方向）
    double fy = 400.0;  // 焦距（y方向）
    double cx = 512.0;  // 主点（x方向）
    double cy = 512.0;  // 主点（y方向）

    // 旋转向量和平移向量
    cv::Mat rotationVector = (cv::Mat_<double>(3, 1) << 90, -30, 0);
    cv::Mat translationVector = (cv::Mat_<double>(3, 1) <<800, 800 ,500);

   // cv::Mat translationVector = (cv::Mat_<double>(3, 1) << 0, 0, 0);

    // 三维点集
    std::vector<cv::Point3d> worldPoints;
    Read3dPoints reader;
    reader.readpoint(worldPoints);
    // 投影后的二维坐标
    std::vector<cv::Point2d> imagePoints;

    // 旋转矩阵
    cv::Mat rotationMatrix;
    cv::Rodrigues(rotationVector, rotationMatrix);
    cout << "R: " << rotationMatrix << endl<<endl;
    //cv::Mat rotationMatrix = (cv::Mat_<double>(3, 3) << 1, 0, 0, 0, 1, 0, 0, 0, 1);
    // 遍历三维点集，并进行投影
    for (const auto& point3d : worldPoints) {

        // 将三维点从世界坐标系转换到相机坐标系
        cv::Mat point3dMat = (cv::Mat_<double>(3, 1) << point3d.x, point3d.y, point3d.z);
        cv::Mat rotatedPoint = rotationMatrix * point3dMat + translationVector;

        // 进行投影
        double x = rotatedPoint.at<double>(0, 0);
        double y = rotatedPoint.at<double>(1, 0);
        double z = rotatedPoint.at<double>(2, 0);
        double u = fx * x / z + cx;
        double v = fy * y / z + cy;

        // 添加高斯噪声
        std::random_device rd;
        std::mt19937 gen(rd());
        std::normal_distribution<double> noise(0.0, 1.0);
        u += noise(gen);
        v += noise(gen);

        // 存储投影后的二维坐标
        imagePoints.push_back(cv::Point2d(u, v));
    }

    // 保存二维点坐标到文件
    std::ofstream outFile("/home/shaohua/highPrecisionLocation/data/Output/one_dim02_data.txt");
    if (outFile.is_open()) {
        for (const auto& point2d : imagePoints) {
            outFile << point2d.x << " " << point2d.y << std::endl;
        }
        outFile.close();
        std::cout << "Image points saved to 'one_dim02_data.txt'" << std::endl;
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
        return 1;
    }

    return 0;
}
