#include "Read2dAnd2d.h"

Read2dAnd2d::Read2dAnd2d() {
}
Read2dAnd2d:: ~Read2dAnd2d() {}

void Read2dAnd2d::readpoint(vector<Point2d>& cam1_pts_2d,vector<Point2d>& cam2_pts_2d) {
	
	FILE* fptr = fopen("/home/shaohua/highPrecisionLocation/data/points2d_01.txt", "r");
	if (fptr == NULL) {
		std::cout << "Error: unable to open file " << endl;
		return;
	};
	//正式开始读取数据
	fscanf(fptr, "%d", &num_location_1);
	for (int i = 0; i < num_location_1; ++i) {

		fscanf(fptr, "%lf", &p1_u);
		fscanf(fptr, "%lf", &p1_v);
		cam1_pts_2d.push_back(Point2d(p1_u, p1_v));

	}
	//结束读取数据
	fclose(fptr);



	FILE* fptr1 = fopen("/home/shaohua/highPrecisionLocation/data/points2d_02.txt", "r");
	if (fptr1 == NULL) {
		std::cout << "Error: unable to open file " << endl;
		return;
	};
	//正式开始读取数据
	fscanf(fptr1, "%d", &num_location_2);
	for (int i = 0; i < num_location_2; ++i) {

		fscanf(fptr1, "%lf", &p2_u);
		fscanf(fptr1, "%lf", &p2_v);
		cam2_pts_2d.push_back(Point2d(p2_u, p2_v));

	}
	//结束读取数据
	fclose(fptr1);
}

