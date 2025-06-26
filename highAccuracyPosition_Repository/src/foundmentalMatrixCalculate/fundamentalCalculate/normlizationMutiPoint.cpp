#include <iostream>
#include <fstream>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <opencv2/core/core.hpp>
#include "Read2dAnd2d.h"

using namespace Eigen;
using namespace std;

//坐标的归一化处理
void normalizePoints(const vector<Point2d>& points, vector<Point2d>& normalizedPoints, Eigen::Matrix3d& T) {
    
    // 计算所有像素坐标的平均值
    Point2d mean(0, 0);
    for (const auto& point : points) {
        mean += point;
    } 

    mean *= 1.0 / points.size();

    // 计算缩放的尺度
    double sumSqDist = 0;
    for (const auto& point : points) {
        double dist = std::pow(point.x - mean.x, 2) + std::pow(point.y - mean.y, 2);
        sumSqDist += dist;
    }
    double scale = std::sqrt(2.0 * points.size() / sumSqDist);

    // 计算归一化矩阵实现坐标的平移与尺度的放缩
    T = Matrix3d::Identity();
    T(0, 0) = scale;
    T(1, 1) = scale;
    T(0, 2) = -scale * mean.x;
    T(1, 2) = -scale * mean.y;

    // 计算归一化的基础矩阵，并将其存储到normalizedPoints中
    normalizedPoints.resize(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        Eigen::Vector3d homogeneousPoint(points[i].x, points[i].y, 1.0);
        Eigen::Vector3d normalizedHomogeneousPoint = T * homogeneousPoint;
        normalizedPoints[i] = cv::Point2d(normalizedHomogeneousPoint(0) / normalizedHomogeneousPoint(2),
            normalizedHomogeneousPoint(1) / normalizedHomogeneousPoint(2));
    }
}

// 归一化多点法求解基础矩阵
Matrix3d computeFundamentalMatrix(const vector<Point2d>& points1, const vector<Point2d>& points2) {
    // 读入数据，并将其进行归一化处理
    
    std::vector<Point2d> normalizedPoints1, normalizedPoints2;
    Matrix3d T1, T2;
    normalizePoints(points1, normalizedPoints1, T1);
    normalizePoints(points2, normalizedPoints2, T2);
    //// 输出归一化矩阵
    //cout << "T1" << endl << T1 << endl << endl;
    //cout << "T2" << endl << T2 << endl << endl;

    // 构建超定方程
    MatrixXd A(points1.size(), 9);
    for (size_t i = 0; i < points1.size(); ++i) {
        const double& x1 = normalizedPoints1[i].x;
        const double& y1 = normalizedPoints1[i].y;
        const double& x2 = normalizedPoints2[i].x;
        const double& y2 = normalizedPoints2[i].y;
        A(i, 0) = x2 * x1;
        A(i, 1) = x2 * y1;
        A(i, 2) = x2;
        A(i, 3) = y2 * x1;
        A(i, 4) = y2 * y1;
        A(i, 5) = y2;
        A(i, 6) = x1;
        A(i, 7) = y1;
        A(i, 8) = 1;
    }

    // 使用奇异值分解（SVD）来分解矩阵A。
    //SVD将矩阵A分解为三个矩阵的乘积：U、S和V^T
    JacobiSVD<MatrixXd> svd(A, ComputeFullV);
    //在这里，只需要V矩阵的最后一列，因此通过svd.matrixV().col(8)获取了向量f
    VectorXd f = svd.matrixV().col(8);

    // 将向量f重新组合成一个3x3的基础矩阵
    Matrix3d fundamental_matrix_normalized;
    fundamental_matrix_normalized << f(0), f(1), f(2),
        f(3), f(4), f(5),
        f(6), f(7), f(8);

    // 为了满足基础矩阵的约束条件，再次使用SVD对基础进行分解。
    JacobiSVD<Matrix3d> svd_f(fundamental_matrix_normalized, ComputeFullU | ComputeFullV);
    Vector3d singular_values = svd_f.singularValues();
    singular_values(2) = 0.0;  //将奇异值向量的第三个元素设置为0，从而将基础矩阵的秩限制为2，并通过乘积运算重新构建基础矩阵
    fundamental_matrix_normalized = svd_f.matrixU() * singular_values.asDiagonal() * svd_f.matrixV().transpose();

    return fundamental_matrix_normalized;
}

int main() {

    //读入两视图中的匹配点
    vector<Point2d> points1, points2;
    Read2dAnd2d reader;
    reader.readpoint(points1, points2);
   
    //计算归一化多点法的基础矩阵
    Matrix3d F = computeFundamentalMatrix(points1, points2);
    std::cout << "Normalized F1_2 matrix:\n" << F << std::endl;

    // 计算出两个归一化矩阵
    Matrix3d T1, T2;
    std::vector<Point2d> normalizedPoints1, normalizedPoints2;
    normalizePoints(points1, normalizedPoints1, T1);
    normalizePoints(points2, normalizedPoints2, T2);

    //对于归一化的基础矩阵进行反归一化处理，得到基础矩阵
    F = T2.transpose() * F * T1;
    std::cout << "Denormalized F1_2 matrix:\n" << F << std::endl;

    return 0;
}
