#ifndef CAMERA_H
#define CAMERA_H
#include <iostream>
#include "CameraApi.h"
#include <opencv2/core.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <thread>

#include <boost/asio.hpp>   //串口相关

using namespace std;
using namespace cv;
using namespace boost::asio;

class CameraNode    //串口命名空间声明
{
public:
    CameraNode();
    ~CameraNode();
    void closeCamera();

    bool claimGoodsTask(std::string goodsShelf,std::string goodsName);
    bool deliverGoodsTask(std::string carShelf,std::string goodsShelf);
    void cameraThread();
    void cameraDesktopThread();

private:
    void serialPortSent(short data_a1,short data_a2,short data_a3,int times) ; 
    void QRCode(cv::Mat& image,std::vector<std::string>&informations,std::vector<Point2d>&points);
    void LayerControl(Mat& image,string goodsShelf);   
    int  extractNumber(std::string input);
    void layersUpOrDowm(std::string goodsShelf,std::string goalString);
    std::string extractUppercaseLetters(const std::string& input);
    bool strCheack(std::string str1,std::string str2);
    bool positionControl(cv::Mat& image,std::string str);
    void goodsControl(cv::Mat& image,std::string goodsName);
    void writedata(short camerapix_u,short camerapix_v, short cameramode);
    unsigned char getCrc8(unsigned char *ptr, unsigned short len);
    void serialInit();
private:
    cv::Mat  image;
    std::thread th;
    bool cameraStatus;
    bool cameraInitStatus;
    vector<std::string> informations;
    std::vector<Point2d> points;

    //串口相关对象
    boost::system::error_code err;
    boost::asio::io_service iosev;
    serial_port sp;
   
    

    const unsigned char header[2] = {0x55, 0xbb};       //数据帧头


};
#endif 