#include <iostream>
#include <opencv2/opencv.hpp>
#include <random>
#include <fstream>
int main()
{


    // 圆心和半径
    cv::Point3d center(500, 500, 500);
    double radius = 300;

    // 随机数生成器
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> theta_dist(0, 2 * CV_PI);
    std::uniform_real_distribution<double> phi_dist(0, CV_PI);

    // 随机生成点
    int num_points = 3000;
    std::vector<cv::Point3d> points;
    for (int i = 0; i < num_points; ++i) {
        // 在球内随机生成点
        double theta = theta_dist(gen);
        double phi = phi_dist(gen);
        double x = center.x + radius * std::sin(phi) * std::cos(theta);
        double y = center.y + radius * std::sin(phi) * std::sin(theta);
        double z = center.z + radius * std::cos(phi);
        points.push_back(cv::Point3d(x, y, z));
    }
    // 保存点到文件
    std::ofstream file("/home/shaohua/highPrecisionLocation/data/Output/simple3DPoints.txt");
    if (file.is_open()) {
        for (const auto& point : points) {
            file << point.x << " " << point.y << " " << point.z << std::endl;
        }
        file.close();
        std::cout << "Points saved to simple3DPoints.txt" << std::endl;
    }
    else {
        std::cerr << "Unable to open file for writing." << std::endl;
    }

    return 0;
}

