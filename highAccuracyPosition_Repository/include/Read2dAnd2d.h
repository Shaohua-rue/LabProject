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
class Read2dAnd2d {
public:

	Read2dAnd2d();
	~Read2dAnd2d();
	void readpoint(vector<Point2d>& cam1_pts_2d,vector<Point2d>& cam2_pts_2d);
	int num_location_1 = 0;
	double p1_u = 0;
	double p1_v = 0;
	int num_location_2 = 0;
	double p2_u = 0;
	double p2_v = 0;

};