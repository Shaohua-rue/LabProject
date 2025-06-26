#include <iostream>
#include <vector>
#include <Eigen/Core>
#include <Eigen/Dense>

Eigen::Vector3d triangulation(const Eigen::Vector2d& p1, const Eigen::Vector2d& p2,
    const Eigen::Matrix3d& K1, const Eigen::Matrix3d& R1, const Eigen::Vector3d& t1,
    const Eigen::Matrix3d& K2, const Eigen::Matrix3d& R2, const Eigen::Vector3d& t2)
{
    // 构造投影矩阵
    Eigen::Matrix<double, 3, 4> P1, P2;

    Eigen::Matrix<double, 3, 3> KR1 = K1 * R1;
    Eigen::Matrix<double, 3, 1> Kt1 = K1 * t1;
    P1 << KR1, Kt1.replicate(1, 1); // 进行矩阵的堆叠

    Eigen::Matrix<double, 3, 3> KR2 = K2 * R2;
    Eigen::Matrix<double, 3, 1> Kt2 = K2 * t2;
    P2 << KR2, Kt2.replicate(1, 1); // 进行矩阵的堆叠

    /* 构造A矩阵 */
    Eigen::Matrix<double, 4, 4> A;
    // 对A的每一列分别进行赋值
    for (int i = 0; i < 4; i++) {
        // 第1个点
        A(0, i) = p1[0] * P1(2, i) - P1(0, i);
        A(1, i) = p1[1] * P1(2, i) - P1(1, i);

        // 第2个点
        A(2, i) = p2[0] * P2(2, i) - P2(0, i);
        A(3, i) = p2[1] * P2(2, i) - P2(1, i);
    }
    // 假设在此处对矩阵A进行初始化

    Eigen::JacobiSVD<Eigen::MatrixXd> svd(A, Eigen::ComputeFullV);
    Eigen::MatrixXd V = svd.matrixV();

    Eigen::Vector3d X;
    X[0] = V(0, 3) / V(3, 3);
    X[1] = V(1, 3) / V(3, 3);
    X[2] = V(2, 3) / V(3, 3);

    return X;
}

bool is_correct_pose(const Eigen::Matrix3d& R1, const Eigen::Vector3d& t1,
    const Eigen::Matrix3d& R2, const Eigen::Vector3d& t2)
{
    /* 相机内参矩阵 */
    Eigen::Matrix3d K1, K2;
    K1 << 1844.03168755262, 0, 945.294134097103,
        0, 1844.07770322705, 741.4045128828183,
        0, 0, 1;
    K2 << 1.842020703544189e+03, 0, 9.653064693225723e+02,
        0, 1.847035333612887e+03, 7.109314273383399e+02,
        0, 0, 1;

    Eigen::Vector2d p2 = { 858.497236, 802.475488 };
    Eigen::Vector2d p1 = { 950.477625, 613.106148 };

    Eigen::Vector3d p3d = triangulation(p1, p2, K1, R1, t1, K2, R2, t2);
    Eigen::Vector3d x1 = R1 * p3d + t1;
    Eigen::Vector3d x2 = R2 * p3d + t2;
    return x1[2] > 0 && x2[2] > 0;
}

