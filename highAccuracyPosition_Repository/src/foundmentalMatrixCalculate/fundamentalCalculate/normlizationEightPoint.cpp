#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <fstream>
#include <chrono>
#include <cstdio>
#include <vector>
#include <string>
#include <Eigen/Core>
#include <Eigen/Dense>
#include <opencv2/core/eigen.hpp>
#include "Read2dAnd2d.h"

using namespace Eigen;
using namespace std;
using namespace cv;

void normalizePoints(const vector<Point2d>& points, vector<Point2d>& normalizedPoints, Matrix3d& T) {
    // Calculate mean of points
    Point2d mean(0, 0);
    for (const auto& point : points) {
        mean += point;
    }
    mean *= 1.0 / points.size();

    // Calculate scale factor
    double sumSqDist = 0;
    for (const auto& point : points) {
        double dist = pow(point.x - mean.x, 2) + pow(point.y - mean.y, 2);
        sumSqDist += dist;
    }
    double scale = sqrt(2.0 * points.size() / sumSqDist);

    // Compute normalization matrix T
    T = Matrix3d::Identity();
    T(0, 0) = scale;
    T(1, 1) = scale;
    T(0, 2) = -scale * mean.x;
    T(1, 2) = -scale * mean.y;

    // Normalize points
    normalizedPoints.resize(points.size());
    for (size_t i = 0; i < points.size(); ++i) {
        Vector3d homogeneousPoint(points[i].x, points[i].y, 1.0);
        Vector3d normalizedHomogeneousPoint = T * homogeneousPoint;
        normalizedPoints[i] = Point2d(normalizedHomogeneousPoint(0) / normalizedHomogeneousPoint(2),
            normalizedHomogeneousPoint(1) / normalizedHomogeneousPoint(2));
    }
}

int main(int argc, char** argv) {
    Read2dAnd2d reader;

    vector<Point2d> cam1_pts_2d;
    vector<Point2d> cam2_pts_2d;

    reader.readpoint(cam1_pts_2d, cam2_pts_2d);

    // Normalize the points for cam1
    Matrix3d T1;
    normalizePoints(cam1_pts_2d, cam1_pts_2d, T1);

    // Normalize the points for cam2
    Matrix3d T2;
    normalizePoints(cam2_pts_2d, cam2_pts_2d, T2);

    // Output the normalization transformation matrices and normalized coordinates
    cout << "T1" << endl << T1 << endl << endl;
    cout << "T2" << endl << T2 << endl << endl;

    // Build the augmented matrix A
    MatrixXd A(8, 9);
    for (int i = 0; i < 8; i++) {
        const double& u1 = cam1_pts_2d[i].x;
        const double& v1 = cam1_pts_2d[i].y;
        const double& u2 = cam2_pts_2d[i].x;
        const double& v2 = cam2_pts_2d[i].y;
        A(i, 0) = u2 * u1;
        A(i, 1) = u2 * v1;
        A(i, 2) = u2;
        A(i, 3) = v2 * u1;
        A(i, 4) = v2 * v1;
        A(i, 5) = v2;
        A(i, 6) = u1;
        A(i, 7) = v1;
        A(i, 8) = 1.0;
    }

    // Perform singular value decomposition (SVD) on A
    JacobiSVD<MatrixXd> svd(A, ComputeFullV);
    VectorXd f = svd.matrixV().col(8);

    // Reshape f into the 3x3 fundamental matrix F
    Matrix3d fundamental_matrix_normalized;
    fundamental_matrix_normalized << f(0), f(1), f(2),
        f(3), f(4), f(5),
        f(6), f(7), f(8);

    // Enforce the rank-2 constraint on the fundamental matrix
    JacobiSVD<Matrix3d> svd_f(fundamental_matrix_normalized, ComputeFullU | ComputeFullV);
    Vector3d singular_values = svd_f.singularValues();
    singular_values(2) = 0.0;  // Set the third singular value to zero
    fundamental_matrix_normalized = svd_f.matrixU() * singular_values.asDiagonal() * svd_f.matrixV().transpose();
    cout << "Fundamental matrix:" << endl << fundamental_matrix_normalized << endl;

    // Denormalize the fundamental matrix
    Matrix3d fundamental_matrix = T2.transpose() * fundamental_matrix_normalized * T1;

    cout << "Denormalized fundamental matrix:" << endl << fundamental_matrix << endl;

    return 0;
}

