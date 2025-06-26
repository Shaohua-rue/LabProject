#include "homoRead2dAnd2d.h"

homoRead2dAnd2d::homoRead2dAnd2d() {
}
homoRead2dAnd2d:: ~homoRead2dAnd2d() {}

void homoRead2dAnd2d::readpoint(vector<Vector3d>& cam1_pts_3d, vector<Vector3d>& cam2_pts_3d) {

	FILE* fptr = fopen("/home/shaohua/highPrecisionLocation/data/points2d_01.txt", "r");
	if (fptr == NULL) {
		std::cout << "Error: unable to open file " << endl;
		return;
	};
	//正式开始读取数据
	fscanf(fptr, "%d", &num_location_1);
	for (int i = 0; i < num_location_1; ++i) {

		fscanf(fptr, "%lf", &p1_x);
		fscanf(fptr, "%lf", &p1_y);
		cam1_pts_3d.push_back(Vector3d(p1_x, p1_y,1.0));

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

		fscanf(fptr1, "%lf", &p2_x);
		fscanf(fptr1, "%lf", &p2_y);
		cam2_pts_3d.push_back(Vector3d(p2_x, p2_y,1.0));

	}
	//结束读取数据
	fclose(fptr1);
}


