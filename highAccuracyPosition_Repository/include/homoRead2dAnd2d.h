#include <fstream>
#include <string>
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include <Eigen/Dense>

using namespace std;
using namespace cv;
using namespace Eigen;

class homoRead2dAnd2d {
public:

	homoRead2dAnd2d();
	~homoRead2dAnd2d();
	void readpoint(vector<Vector3d>& cam1_pts_3d, vector<Vector3d>& cam2_pts_3d);
	int num_location_1 = 0;
	double p1_x = 0;
	double p1_y = 0;
	double p1_z = 0;
	int num_location_2 = 0;
	double p2_x = 0;
	double p2_y = 0;
	double p2_z = 0;
};
#pragma once

