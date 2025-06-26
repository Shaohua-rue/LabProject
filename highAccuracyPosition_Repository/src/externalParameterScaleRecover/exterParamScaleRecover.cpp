#include <iostream>
#include <Eigen/Dense>
#include "Read3dPoints.h"
using namespace std;
using namespace Eigen;
/******************************************************************************
*
* 此程序为利用一维标定杆恢复外参中平移向量的尺度因子，并将真实三维坐标输出
* 
* written by ShaoHua
*******************************************************************************/
MatrixXd MyrecoverScale(const MatrixXd& worldPoint, double constrains_length) {
    int n = worldPoint.rows();
    int numPoints = n / 3;

    VectorXd scale_vec(numPoints);
    MatrixXd worldPointWithScale = worldPoint;
    VectorXd xyz_square(numPoints);
    VectorXd weight(numPoints);
    double a = 0.0;
    double scale = 0.0;
    int j = 0;
    int z = 0;
    for (int i = 0; i < numPoints; i++) {
        int index1 = j;
        int index2 = j + 1;
        int index3 = j + 2;
        xyz_square(i) = pow(worldPoint(index1, 0) - worldPoint(index3, 0), 2.0) +
            pow(worldPoint(index1, 1) - worldPoint(index3, 1), 2.0) +
            pow(worldPoint(index1, 2) - worldPoint(index3, 2), 2.0);

        scale_vec(i) = constrains_length / sqrt(xyz_square(i));

        Vector3d ca = worldPoint.row(index1) - worldPoint.row(index2);
        Vector3d cb = worldPoint.row(index3) - worldPoint.row(index2);
        //double k = 0.67;
        //double t =( k*ca.norm() / cb.norm());
       // double g_t = (t > 1.0) ? (1.0 / t) : t;

        //weight(i) = g_t*(1.0 - ca.dot(cb) / (ca.norm() * cb.norm()));
        weight(i) = (1.0 - ca.dot(cb) / (ca.norm() * cb.norm()));
        a += weight(i);
        j = j + 3;
    }

    for (int i = 0; i < numPoints; i++) {
        scale += scale_vec(i) * weight(i) / a;
    }
    cout << "恢复的尺度因子：" << scale << endl;
    for (int i = 0; i < numPoints; i++) {
        int index1 = z;
        int index2 = z + 1;
        int index3 = z + 2;

        worldPointWithScale(index1, 0) *= scale;
        worldPointWithScale(index1, 1) *= scale;
        worldPointWithScale(index1, 2) *= scale;

        worldPointWithScale(index2, 0) *= scale;
        worldPointWithScale(index2, 1) *= scale;
        worldPointWithScale(index2, 2) *= scale;

        worldPointWithScale(index3, 0) *= scale;
        worldPointWithScale(index3, 1) *= scale;
        worldPointWithScale(index3, 2) *= scale;
        z = z + 3;
    }
    return worldPointWithScale;
}

int main() {
   
    vector<Point3d> cam1_pts_3d; 
    Read3dPoints reader;
    reader.readpoint(cam1_pts_3d);
    int m = cam1_pts_3d.size();
    MatrixXd worldPoint(m, 3);
    for (int i = 0; i < m; i++) {
        worldPoint.row(i) << cam1_pts_3d[i].x, cam1_pts_3d[i].y, cam1_pts_3d[i].z;
    }
    double constrains_length = 50;
    MatrixXd result = MyrecoverScale(worldPoint, constrains_length);

    cout << "Scaled World Point:\n";
    cout << result << endl;

    return 0;
}

