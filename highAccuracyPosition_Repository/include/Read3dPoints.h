#include <iostream>
#include <fstream>
#include <string>

#include<opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace std;
using namespace cv;

class Read3dPoints {
public:
	Read3dPoints();
	~Read3dPoints();
	void readpoint( vector<Point3d>& cam1_pts_3d);
	int num_location_ = 0;
	double p1_x = 0;
	double p1_y = 0;
	double p1_z = 0;
};
