#ifndef COMMON_H
#define COMMON_H

#include<Eigen/Core>
#include<chrono>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
using namespace Eigen;

chrono::steady_clock::time_point timePoint();
int timeUsed(chrono::steady_clock::time_point t1, chrono::steady_clock::time_point t2);
void outPutPoints2f(vector<Point2f>& points_2f);
void drawPoints(Mat& image_bgr,vector<Point2f>points_2d,bool state);
void drawPointAndLine(Mat& image_bgr, vector<Point2f>& points2d_01, vector<Point2f>& points2d_02);
void rectValueChoose(vector<Point2f> points_2f, float u, float v, float& X_min, float& X_max, float& Y_min, float& Y_max,int size=100);
void fastpoint(cv::Mat gray, int threshold, int window, int pointNum, std::vector<Point2f>& kp);
void selectMax(int window, cv::Mat gray, std::vector<KeyPoint>& kp);

//------------------------------------算法检测用时函数-------------------------------
chrono::steady_clock::time_point timePoint()
{
    return chrono::steady_clock::now();
}

int timeUsed(chrono::steady_clock::time_point t1, chrono::steady_clock::time_point t2)
{
    auto time_used = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);
    return time_used.count();
}


//------------------------------------角点输出与输出------------------------------------------
void outPutPoints2f(vector<Point2f>& points_2f)
{
    for (int i = 0; i < points_2f.size(); i++)
    {
        cout << "第" << i + 1 << "个角点坐标为：[" << points_2f[i].x << "," << points_2f[i].y<<"]" << endl;
    }
}
void drawPoints(Mat& image_bgr,vector<Point2f>points_2d,bool state)
{
    for (int i = 0; i < points_2d.size(); i++)
    {
        Point2f point_2d = points_2d[i];
        //void circle(InputOutputArray img, Point center, int radius(圆的半径),const Scalar& color, int thickness = 1(圆的边界粗细),int lineType = LINE_8(线型), int shift = 0(坐标点的小数位数));
        circle(image_bgr, point_2d, 5, Scalar(255, 0, 0), 4, 8, 0);
        if (state)
        {
        //void putText( InputOutputArray img, const String& text(绘制的文本内容), Point org, int fontFace(字体类型), float fontScale(字体的缩放因子), Scalar color,int thickness = 1(文本粗细), int lineType = LINE_8(线型),bool bottomLeftOrigin = false(文本是否在左下角) );
            putText(image_bgr, to_string(i + 1), point_2d, 1, 2, Scalar(0, 0, 255), 2, LINE_4);
        }
    }
}
void drawPointAndLine(Mat& image_bgr, vector<Point2f>& points2d_01, vector<Point2f>& points2d_02)
{
    for (int i = 0; i < points2d_02.size(); i++)
    {
        Point2f pt1 = points2d_01[i];
        Point2f pt2 = points2d_02[i];
        circle(image_bgr, pt2, 4, Scalar(255, 0, 0), 4, 8, 0);
        cv::line(image_bgr, pt1, pt2, cv::Scalar(0, 250, 0),2);

    }
}

void rectValueChoose(vector<Point2f> points_2f, float u, float v, float& X_min, float& X_max, float& Y_min, float& Y_max,int rectSize)
{
    float x_min = points_2f[0].x;
    float x_max = points_2f[0].x;
    float y_min = points_2f[0].y;
    float y_max = points_2f[0].y;

    //检测第一帧角点x，y的最大与最小值
    for (int i = 1; i < points_2f.size(); i++)
    {
        Point2f current = points_2f[i];
        if (current.x < x_min)
        {
            x_min = current.x;
        }
        else if (current.x > x_min)
        {
            x_max = current.x;
        }

        if (current.y < y_min)
        {
            y_min = current.y;
        }
        else if (current.y > y_min)
        {
            y_max = current.y;
        }

    }
    cout << x_min << " " << x_max << y_min << " " << y_max << endl;
    x_min -= rectSize; y_min -= rectSize; x_max += rectSize; y_max += rectSize;




    //矩形框左侧未超出图片左侧
    if (x_min > 0)
    {
        //矩形框上侧未超出图片上侧
        if (y_min > 0)
        {   //矩形框右侧未超过图片右侧
            if (x_max < u)
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形在图片中" << endl;
                    cout << "x_min" << x_min << "  y_min" << y_min << " x_max " << x_max << " y_max" << y_max << endl;  //ok
                    X_min = x_min;  Y_min = y_min; X_max = x_max; Y_max = y_max;
                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片下侧" << endl;
                    cout << "x_min" << x_min << "  y_min" << y_min << " x_max " << x_max << " y_max" << v << endl;      //ok
                    X_min = x_min;  Y_min = y_min; X_max = x_max; Y_max = v;

                }

            }
            //矩形框右侧超过图片右侧
            else
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形右侧超过图片右侧" << endl;
                    cout << "x_min" << x_min << "  y_min" << y_min << " x_max " << u << " y_max" << y_max << endl;  //ok
                    X_min = x_min;  Y_min = y_min; X_max = u; Y_max = y_max;
                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片右侧与下侧" << endl;
                    cout << "x_min" << x_min << "  y_min" << y_min << " x_max " << u << " y_max" << v << endl;  //ok
                    X_min = x_min;  Y_min = y_min; X_max = u; Y_max = v;
                }
            }
        }
        //矩形框上侧超出图片上侧
        else
        {
            //矩形框右侧未超过图片右侧
            if (x_max < u)
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形超过图片上侧" << endl;
                    cout << "x_min" << x_min << "  y_min" << 0 << " x_max " << x_max << " y_max" << y_max << endl;//ok
                    X_min = x_min; Y_min = 0;  X_max = x_min; Y_max = y_max;
                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片上侧与下侧" << endl;
                    cout << "x_min" << x_min << "  y_min" << 0 << " x_max " << x_max << " y_max" << v << endl;  //ok
                    X_min = x_min; Y_min = 0;  X_max = x_min; Y_max = v;
                }

            }
            //矩形框右侧超过图片右侧
            else
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形超过图片上侧与右侧" << endl;
                    cout << "x_min" << x_min << "  y_min" << 0 << " x_max " << u << " y_max" << y_max << endl;  //ok
                    X_min = x_min; Y_min = 0;  X_max = u; Y_max = y_max;

                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片上侧与右侧与下侧" << endl;
                    cout << "x_min" << x_min << "  y_min" << 0 << " x_max " << u << " y_max" << v << endl;  //ok
                    X_min = x_min;  Y_min = 0; X_max = u; Y_max = v;
                }
            }

        }
    }
    //矩形框左侧超出图片左侧
    else
    {
        //矩形框上侧未超出图片上侧
        if (y_min > 0)
        {   //矩形框右侧未超过图片右侧
            if (x_max < u)
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形超过图片左侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << y_min << " x_max " << x_max << " y_max" << y_max << endl;  //ok
                    X_min = 0;  Y_min = y_min; X_max = x_max; Y_max = y_max;
                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片左侧与下侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << y_min << " x_max " << x_max << " y_max" << v << endl;      //ok
                    X_min = 0;  Y_min = y_min; X_max = x_max; Y_max = v;
                }

            }
            //矩形框右侧超过图片右侧
            else
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形右侧超过图片左侧右侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << y_min << " x_max " << u << " y_max" << y_max << endl;  //ok
                    X_min = 0;  Y_min = y_min; X_max = u; Y_max = y_max;
                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片左侧与右侧与下侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << y_min << " x_max " << u << " y_max" << v << endl;  //ok
                    X_min = 0;  Y_min = y_min; X_max = u; Y_max = v;
                }
            }
        }
        //矩形框上侧超出图片上侧
        else
        {
            //矩形框右侧未超过图片右侧
            if (x_max < u)
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形超过图片左侧与上侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << 0 << " x_max " << x_max << " y_max" << y_max << endl;//ok
                    X_min = 0;  Y_min = 0; X_max = x_max; Y_max = y_max;
                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片左侧与上侧与下侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << 0 << " x_max " << x_max << " y_max" << v << endl;  //ok
                    X_min = 0;  Y_min = 0; X_max = x_max; Y_max = v;
                }

            }
            //矩形框右侧超过图片右侧
            else
            {
                //矩形框下侧未超过图片下侧
                if (y_max < v)
                {
                    cout << "矩形超过图片左侧与上侧与右侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << 0 << " x_max " << u << " y_max" << y_max << endl;  //ok
                    X_min = 0;  Y_min = 0; X_max = u; Y_max = y_max;

                }
                //矩形框下侧超过图片下侧
                else
                {
                    cout << "矩形超过图片左侧与上侧与右侧与下侧" << endl;
                    cout << "x_min" << 0 << "  y_min" << 0 << " x_max " << u << " y_max" << v << endl;  //ok
                    X_min = 0;  Y_min = 0; X_max = u; Y_max = v;
                }
            }

        }

    }

}
//进行非极大值抑制
void selectMax(int window, cv::Mat gray, std::vector<KeyPoint>& kp) {

    //window是局部极大值抑制的窗口大小，r为半径
    int r = window / 2;
    if (window != 0) {
        //对kp中的点进行局部极大值筛选
        for (int i = 0; i < kp.size(); i++) {
            for (int j = i + 1; j < kp.size(); j++) {
                //如果两个点的距离小于半径r，则删除其中响应值较小的点
                if (abs(kp[i].pt.x - kp[j].pt.x) + abs(kp[i].pt.y - kp[j].pt.y) <= 2 * r) {
                    if (kp[i].response < kp[j].response) {
                        std::vector<KeyPoint>::iterator it = kp.begin() + i;
                        kp.erase(it);
                        selectMax(window, gray, kp);
                    }
                    else {
                        std::vector<KeyPoint>::iterator it = kp.begin() + j;
                        kp.erase(it);
                        selectMax(window, gray, kp);
                    }
                }
            }
        }
    }

}
//进行fast角点检测
void fastpoint(cv::Mat gray, int threshold, int window, int pointNum, std::vector<Point2f>& kp) {

    std::vector<KeyPoint> keypoint;

    cv::FAST(gray, keypoint, 100);

    if (keypoint.size() > pointNum) {
        threshold = threshold + 5;
        fastpoint(gray, threshold, window, pointNum, kp);
    }
    selectMax(window, gray, keypoint);
    for (int i = 0; i < keypoint.size(); i++)
    {
        kp.push_back(keypoint[i].pt);

    }
    //kp.assign(keypoint.begin(), keypoint.end());

}

#endif // COMMON_H
