#include <iostream>
#include <vector>
#include <opencv2/core.hpp>
#include <opencv2/calib3d.hpp>
#include <Eigen/Dense>
#include "homoRead2dAnd2d.h"

using namespace std;
using namespace cv;
using namespace Eigen;
/*******************************************************************************************
*
* 此程序为双目三角化求解世界坐标
* 
* written by ShaoHua
*********************************************************************************************/

// 将像素坐标系下的坐标，转换到归一化坐标系下的坐标，(u, v) - (x, y, 1)
// x = (u - cx) / fx
// y = (v - cy) / fy
Point2f pixel2cam(const Point2d& p, const Mat& K) {
    return Point2f(
        (p.x - K.at<double>(0, 2)) / K.at<double>(0, 0),
        (p.y - K.at<double>(1, 2)) / K.at<double>(1, 1)
    );
}


void triangulation(const vector<Vector3d>& x1, const vector<Vector3d>& x2,
    const Matrix3d& K1, const Matrix3d& K2, Matrix3d& R, const Vector3d& t, vector<Vector3d>& points) {

// 默认以第一幅图像的相机坐标系为基准，还原出来的深度信息，就是相对于第一幅图像视角下的深度
    Mat T1 = (Mat_<float>(3, 4) <<
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0);
    Mat T2 = (Mat_<float>(3, 4) <<
        R(0, 0), R(0, 1), R(0, 2), t(0),
        R(1, 0), R(1, 1), R(1, 2), t(1),
        R(2, 0), R(2, 1), R(2, 2), t(2));

    vector<Point2f> pts_1, pts_2;

// 将像素坐标转换成归一化平面坐标，存储于pts_1、pts_2
    int numPoints = x1.size();
    for (int i = 0; i < numPoints; i++) {
        Eigen::Vector2d p1(x1[i][0], x1[i][1]);
        Mat K_cv1 = Mat::zeros(3, 3, CV_64F);
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                K_cv1.at<double>(r, c) = K1(r, c);
            }
        }
        pts_1.push_back(pixel2cam(Point2d(p1[0], p1[1]), K_cv1));
    }
    for (int i = 0; i < numPoints; i++) {
        Eigen::Vector2d p2(x2[i][0], x2[i][1]);
        Mat K_cv2 = Mat::zeros(3, 3, CV_64F);
        for (int r = 0; r < 3; r++) {
            for (int c = 0; c < 3; c++) {
                K_cv2.at<double>(r, c) = K2(r, c);
            }
        }
        pts_2.push_back(pixel2cam(Point2d(p2[0], p2[1]), K_cv2));
    }
// cv::triangulatePoints函数，输出的3D坐标是齐次坐标，共四个维度，因此需要将前三个维度除以第四个维度，得到非齐次坐标x、y、z
    Mat pts_4d;
    cv::triangulatePoints(T1, T2, pts_1, pts_2, pts_4d);

    // 转换成非齐次坐标
    for (int i = 0; i < pts_4d.cols; i++) {
        Mat x = pts_4d.col(i);
        x /= x.at<float>(3, 0);// 归一化，四个分量全除以最后一位，将第四位数值转化为1
        Vector3d p(x.at<float>(0, 0), x.at<float>(1, 0), x.at<float>(2, 0));
        points.push_back(p);
    }
}

int main() {

// 假设已经有匹配好的2D点对 x1 和 x2
    vector<Vector3d> x1;
    vector<Vector3d> x2;
    homoRead2dAnd2d reader;
    reader.readpoint(x1, x2);

// 相机内参矩阵和外参（旋转矩阵R和平移向量t）
    //Matrix3d K1;
    //K1 << 1.844031687552622e+03, -0.246454815039132, 9.452941340971031e+02,
    //    0, 1.844077703227055e+03, 7.414045128828183e+02,
    //    0, 0, 1;
    //Matrix3d K2;
    //K2 << 1.842020703544189e+03, -0.029889011366445, 9.653064693225723e+02,
    //    0, 1.847035333612887e+03, 7.109314273383399e+02,
    //    0, 0, 1;
    //Matrix3d R;
    //R << 0.2329059624036369, 0.479451947255471, -0.8460973011124506,
    //-0.4816925086990212, 0.8126761101801547, 0.3279174698086343,
    //0.8448237329408301, 0.3311847976815819, 0.4202255228380716;
    //
    //Vector3d t(3492.09, - 960.242 , 2073.02);
    
    Matrix3d K1;
    K1 << 400, 0, 512, 0, 400, 512, 0, 0, 1;
    Matrix3d K2;
    K2 << 400, 0, 512, 0, 400, 512, 0, 0, 1;
    Matrix3d R;
    R << 0.9781829945469229, -0.05978149074713215, -0.1989577908584258,
        -0.06554483273446463, 0.8199751132238442, -0.5686340550787959,
        0.1971342285881009, 0.5692688179213706, 0.7981673438956143;

    Vector3d t(816.068, 818.409, 485.228);
// 计算深度信息
    vector<Vector3d> points;
    triangulation(x1, x2, K1, K2, R, t, points);

// 打印每个点的深度信息
    for (int i = 0; i < points.size(); i++) {

        cout << points[i](0) << "  "<< points[i](1) << "  " << points[i](2) << " " << endl;
    }

    return 0;
}

