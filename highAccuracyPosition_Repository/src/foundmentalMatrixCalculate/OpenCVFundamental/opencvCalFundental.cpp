#include "Read2dAnd2d.h"
using namespace std;
using namespace cv;

int main() {
    // 左图像中的特征点坐标
    vector<Point2d> points1;
    // 右图像中的特征点坐标
    vector<Point2d> points2;
    Read2dAnd2d reader;
    reader.readpoint(points1, points2);


    // 使用LMedS算法估计 左图像到右图像的基础矩阵，即F1_2
    Mat fundamentalMatrix = findFundamentalMat(points1, points2, FM_LMEDS);

    // 输出计算得到的基础矩阵
    cout << "LMEDS F1_2:\n" << fundamentalMatrix << endl;
   
    return 0;
}