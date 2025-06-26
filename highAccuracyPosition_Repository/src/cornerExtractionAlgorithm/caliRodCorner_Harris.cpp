#include <opencv2/opencv.hpp>
#include <iostream>
#include <fstream>

using namespace std;
using namespace cv;


int main() {

	std::string imagePath = "/home/shaohua/highPrecisionLocation/data/Picture/010.BMP";
	cv::Mat image = cv::imread(imagePath);



	// 转换为灰度图像
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);

	// 进行腐蚀操作，以减少噪声
	Mat erodedImage;
	Mat element = getStructuringElement(MORPH_RECT, Size(6, 6), Point(-1, -1));   //自定义核，主要更改Size(5, 5)
	erode(grayImage, erodedImage, element);    //进行腐蚀操作


	std::vector<cv::Point2f> corners;
	std::vector<cv::Point2f> Corner;
	// 使用Shi-Tomasi角点检测算法
	cv::goodFeaturesToTrack(erodedImage, corners, 100, 0.01, 10, cv::Mat(), 3, false, 0.04);


	// 使用亚像素角点算法来提高角点精度
	cv::TermCriteria subpixelCriteria(cv::TermCriteria::EPS + cv::TermCriteria::MAX_ITER, 30, 0.001);
	cv::cornerSubPix(erodedImage, corners, cv::Size(11, 11), cv::Size(-1, -1), subpixelCriteria);

	float dist01 = norm(corners[0] - corners[1]);
	float dist12 = norm(corners[1] - corners[2]);
	float dist02 = norm(corners[0] - corners[2]);
	cout << dist01 << "  " << dist12 << "   " << dist02 << endl;

	if((dist01 < dist12)&& (dist12< dist02)&& (dist01 < dist02))
	{

		Corner.push_back(corners[0]);
		Corner.push_back(corners[1]);
		Corner.push_back(corners[2]);
		cout << "dist01 < dist12 < dist02" << endl;
	}
	
	else if ((dist02 < dist12) && (dist12 < dist01) && (dist02 < dist01))
	{

		Corner.push_back(corners[0]);
		Corner.push_back(corners[2]);
		Corner.push_back(corners[1]);
		cout << "dist02 < dist12 < dist01" << endl;

	}
	else if ((dist01 < dist02) && (dist02 < dist12) && (dist01 < dist12))
	{

		Corner.push_back(corners[1]);
		Corner.push_back(corners[0]);
		Corner.push_back(corners[2]);
		cout << "dist01 < dist02 < dist12" << endl;

	}
	
	else if ((dist12 < dist02) && (dist02 < dist01) && (dist12 < dist01))
	{

		Corner.push_back(corners[1]);
		Corner.push_back(corners[2]);
		Corner.push_back(corners[0]);
		cout << "dist12 < dist02 < dist01" << endl;

	}
	
	else if ((dist02 < dist01) && (dist01 < dist12) && (dist02 < dist12))
	{

		Corner.push_back(corners[2]);
		Corner.push_back(corners[0]);
		Corner.push_back(corners[1]);
		cout << "dist02 < dist01 < dist12" << endl;
	}

	else if ((dist12 < dist01) && (dist01 < dist02) && (dist12 < dist02))
	{

		Corner.push_back(corners[2]);
		Corner.push_back(corners[1]);
		Corner.push_back(corners[0]);
		cout << "dist12 < dist01 < dist02" << endl;

	}
	for (int i = 0; i < Corner.size(); i++)
	{
		cout << Corner[i].x << "   " << Corner[i].y << std::endl;
		putText(image, to_string(i), Corner[i], 1, 4, Scalar(255, 0, 0), 4, LINE_4);
		circle(image, Corner[i], 3, Scalar(255, 0, 0), 2, 8, 0);
	}

	namedWindow("Dection picture", WINDOW_NORMAL);
	imshow("Dection picture", image);


	waitKey(0);
	return 0;
}


