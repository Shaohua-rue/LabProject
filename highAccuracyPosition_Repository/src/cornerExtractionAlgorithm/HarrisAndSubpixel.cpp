#include <iostream> 
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

/****************************************************
*
* 此程序为手写Harris角点检测与曲面拟合法求解亚像素坐标
*
* Written By Shaohua
******************************************************/


void subpixel(double x, double y, Mat r, double& subx, double& suby);

class Point2D {
public:
	double x;
	double y;
	Point2D(double x, double y) : x(x), y(y) {}
};

Mat detectHarrisCorners(const Mat& imgSrc, double alpha)
{
	Mat gray;
	vector <int> posc, posr;
	vector<double>pointx;
	vector<double>pointy;

	gray = imgSrc.clone();
	gray.convertTo(gray, CV_64F);  //更改目标图像的数据类型（深度），如CV_8U,CV_32F;
	Mat xKernel = (Mat_<double>(1, 5) << -2, -1, 0, 1, 2);//自定义卷积核，用于滤波操作
	Mat yKernel = xKernel.t();

	//1.计算图像在X与Y方向上的梯度
	Mat Ix, Iy;
	filter2D(gray, Ix, CV_64F, xKernel);
	filter2D(gray, Iy, CV_64F, yKernel);

	//2.计算图像方向梯度的乘积
	Mat Ix2, Iy2, Ixy;
	Ix2 = Ix.mul(Ix);
	Iy2 = Iy.mul(Iy);
	Ixy = Ix.mul(Iy);

	//3.进行高斯加权，生成M元素
	GaussianBlur(Ix2, Ix2, cv::Size(15, 15), 5, 5);
	GaussianBlur(Iy2, Iy2, cv::Size(15, 15), 5, 5);
	GaussianBlur(Ixy, Ixy, cv::Size(15, 15), 5, 5);

	//4.计算每个像素值的Harris角点响应值R
	Mat cornerStrength(gray.size(), gray.type());
	Mat r(gray.size(), gray.type());
	for (int i = 0; i < gray.rows; i++)
	{
		for (int j = 0; j < gray.cols; j++)
		{
			double det_m = Ix2.at<double>(i, j) * Iy2.at<double>(i, j) - Ixy.at<double>(i, j) * Ixy.at<double>(i, j);//行列式
			double trace_m = Ix2.at<double>(i, j) + Iy2.at<double>(i, j);//迹
			cornerStrength.at<double>(i, j) = det_m - alpha * trace_m * trace_m;//响应函数值R
			r.at<double>(i, j) = det_m - alpha * trace_m * trace_m;//响应函数值R
		}
	}
	//5.进行非极大值抑制
	double maxStrength;
	minMaxLoc(cornerStrength, NULL, &maxStrength, NULL, NULL);//从一个矩阵中找出全局的最大值和最小值,参数 &maxStrength 返回最大值的指针; 如果不需要输入NULL.
	double qualityLevel = 0.02;
	double thresh = qualityLevel * maxStrength;// 设置threshold

	Mat dilated, localMax;
	//核膨胀，膨胀之后，除了局部最大值点和原来相同，其它非局部最大值点被邻域内的最大值点取代
	//Mat element = getStructuringElement(MORPH_RECT, Size(11, 11), Point(-1, -1));
	Mat element = getStructuringElement(MORPH_RECT, Size(5, 5), Point(-1, -1));
	dilate(cornerStrength, dilated, element);
	//与原图相比，只剩下和原图值相同的点，这些点都是局部最大值点，保存到localMax 
	compare(cornerStrength, dilated, localMax, CMP_EQ);//比较输入的src1和src2中的元素，输出结果到dst中,CMP_EQ:相等
	//和局部最大值图与，剩下角点局部最大值图，即：完成非最大值抑制
	Mat cornerMap;
	cornerMap = cornerStrength > thresh;
	bitwise_and(cornerMap, localMax, cornerMap);//将cornerMap和 localMax每个像素的像素值按位与



	//角点坐标的保存与亚像素坐标的计算
	int cnt = 0;
	for (int y = 0; y < cornerMap.rows; y++)
	{
		const uchar* rowPtr = cornerMap.ptr <uchar>(y);
		for (int x = 0; x < cornerMap.cols; x++)
		{
			//非零点就是角点 
			if (rowPtr[x])
			{
				posc.push_back(y);
				posr.push_back(x);
				cnt++;
			}
		}
	}
	cout << "检测出的角点个数为; " << cnt << " 个" << endl;
	for (int i = 0; i < cnt; i++)
	{
		double y = posr[i];
		double x = posc[i];
		double subx = 0;
		double suby = 0;
		subpixel(x, y, r, subx, suby);         //调用曲面拟合法求解亚像素坐标
		cout << "像素级坐标:#" << i + 1 << " (" << posr[i] << " , " << posc[i] << ")" << endl;
		cout << "亚像素坐标:#" << i + 1 << "(" << subx << "," << suby << ")" << endl;
		cout << "-----------------------------------------------------------------\n" << endl;
		pointx.push_back(subx);
		pointy.push_back(suby);
	}
	vector<Point2D> points2d;
	for (int i = 0; i < cnt; i++)
	{
		Point2D point(pointx[i], pointy[i]);
		points2d.push_back(point);
	}
	sort(points2d.begin(), points2d.end(), [](const Point2D& p1, const Point2D& p2) {
		return sqrt(p1.x * p1.x + p1.y * p1.y) < sqrt(p2.x * p2.x + p2.y * p2.y);
		});



	//根据原点远近对检测到的特征点进行排序，并在图上标注
	cout << "根据原点远近排序的亚像素坐标结果为：" << endl;
	Mat imgRGB = imgSrc.clone();
	for (int i = 0; i < points2d.size(); i++)
	{
		cout << "#" << i + 1 << "(" << points2d[i].x << "  " << points2d[i].y << ") " << endl;
		cout << "-------------------------------" << endl;
		Point p(points2d[i].x, points2d[i].y);
		putText(imgRGB, to_string(i + 1), p, 1, 2, Scalar(255, 0, 0), 4, LINE_4);
	}
	cout << endl;
	//画角点
	cvtColor(imgRGB, imgRGB, COLOR_GRAY2RGB);
	for (int i = 0; i < posc.size(); i++)
	{
		Point pt(posr[i], posc[i]);
		circle(imgRGB, pt, 3, Scalar(255, 0 , 0), 2, 8, 0);

	}
	return imgRGB; //返回标注后的图像·
}

int main()
{

	//读取指定路径下的图像，设置0相当于IMREAD_GRAYSCALE以灰度图像读入,Harris角点检测需要将图片以灰度图像读入；
	//不填或者填1相当于IMREAD_COLOR,以彩色图片读入,填-1相当于IMREAD_UNCHANGED,表示以原样读取图片，如果存在alpha通道也读入；
	Mat image = imread("/home/shaohua/highPrecisionLocation/data/Picture/Camera1.BMP", 0);

	//检测是否正确读取图像
	if (image.data == nullptr)
	{
		cout << "路径错误或文件不存在" << endl;
		return 0;
	}


	//设置窗口显示图像，标志位WINDOW_NORMAL可将图像设置成可调整大小的模式   
	//标志位可设置为固定大小WINDOW_AUTOSIZE，全屏窗口WINDOW_FULLSCREEN,自由比例窗口WINDOW_FRERATIO,保持比例窗口WINDOW_KEEPRATIO;
	namedWindow("转换成的灰度图像", WINDOW_NORMAL);
	imshow("转换成的灰度图像", image);
	Mat element = getStructuringElement(MORPH_RECT, Size(2,2), Point(-1, -1));
	erode(image, image, element);
	Mat HarrisImage = detectHarrisCorners(image, 0.05);//输入的第二个参数为k值，用以调整角点响应函数的敏感度，经验值在0.04-0.06之间

	namedWindow("角点检测结果", cv::WINDOW_NORMAL);
	imshow("角点检测结果", HarrisImage);
	waitKey(0);
	return 0;
}

//曲面拟合法求解亚像素坐标 
void subpixel(double x, double y, Mat r, double& subx, double& suby)
{
	//定义方程pixelmatrix * amatrix = rmatrix
	Mat pixelmatrix = (Mat_<double>(9, 6) <<
		1, x - 1, y - 1, pow(x - 1, 2), (x - 1) * (y - 1), pow(y - 1, 2),
		1, x - 1, y, pow(x - 1, 2), (x - 1) * y, pow(y, 2),
		1, x - 1, y + 1, pow(x - 1, 2), (x - 1) * (y + 1), pow(y + 1, 2),
		1, x, y - 1, pow(x, 2), x * (y - 1), pow(y - 1, 2),
		1, x, y, pow(x, 2), x * y, pow(y, 2),
		1, x, y + 1, pow(x, 2), x * (y + 1), pow(y + 1, 2),
		1, x + 1, y - 1, pow(x + 1, 2), (x + 1) * (y - 1), pow(y - 1, 2),
		1, x + 1, y, pow(x + 1, 2), (x + 1) * y, pow(y, 2),
		1, x + 1, y + 1, pow(x + 1, 2), (x + 1) * (y + 1), pow(y + 1, 2));

	Mat rmatrix(9, 1, CV_64F);
	rmatrix.at<double>(0, 0) = r.at<double>(x - 1, y - 1);
	rmatrix.at<double>(1, 0) = r.at<double>(x - 1, y);
	rmatrix.at<double>(2, 0) = r.at<double>(x - 1, y + 1);
	rmatrix.at<double>(3, 0) = r.at<double>(x, y - 1);
	rmatrix.at<double>(4, 0) = r.at<double>(x, y);
	rmatrix.at<double>(5, 0) = r.at<double>(x, y + 1);
	rmatrix.at<double>(6, 0) = r.at<double>(x + 1, y - 1);
	rmatrix.at<double>(7, 0) = r.at<double>(x + 1, y);
	rmatrix.at<double>(8, 0) = r.at<double>(x + 1, y + 1);
	Mat amatrix(6, 1, CV_64F);

	//求解amatrix
	solve(pixelmatrix, rmatrix, amatrix, DECOMP_NORMAL);
	double a0 = amatrix.at<double>(0, 0);
	double a1 = amatrix.at<double>(1, 0);
	double a2 = amatrix.at<double>(2, 0);
	double a3 = amatrix.at<double>(3, 0);
	double a4 = amatrix.at<double>(4, 0);
	double a5 = amatrix.at<double>(5, 0);

	//求解亚像素坐标
	suby = (2 * a1 * a5 - a2 * a4) / (a4 * a4 - 4 * a3 * a5);
	subx = (2 * a2 * a3 - a1 * a4) / (a4 * a4 - 4 * a3 * a5);

}

