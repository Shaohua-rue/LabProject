#include "Read3dPoints.h"
Read3dPoints::Read3dPoints () {};
Read3dPoints::~Read3dPoints(){};
void Read3dPoints::readpoint( vector<Point3d>& cam1_pts_3d) {
	
	FILE* fptr = fopen("/home/shaohua/highPrecisionLocation/data/points3d.txt", "r");
	if (fptr == NULL) {
		std::cout << "Error: unable to open file " << endl;
		return;
	};
	//正式开始读取数据
	fscanf(fptr, "%d", &num_location_);
	for (int i = 0; i < num_location_; ++i) {

		fscanf(fptr, "%lf", &p1_x);
		fscanf(fptr, "%lf", &p1_y);
		fscanf(fptr, "%lf", &p1_z);
		cam1_pts_3d.push_back(Point3d(p1_x, p1_y, p1_z));
	}
	//结束读取数据
	fclose(fptr);
}
