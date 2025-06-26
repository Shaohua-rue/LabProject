#ifndef IMAGEPROCESS_H
#define IMAGEPROCESS_H
#include <iostream>
#include <ros/ros.h>  
#include <std_msgs/String.h>  
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc_c.h>
#include <cv_bridge/cv_bridge.h>
using namespace std;
using namespace cv;
class ImageProcess
{
    public:
    ImageProcess();
    void run();
    void imageCallback(const sensor_msgs::ImageConstPtr& msg);
    void QRCode(cv::Mat& image);

    private:
    ros::NodeHandle nh_;     // NodeHandle对象，用于与ROS系统交互 
    cv::Mat image_; 

    image_transport::Subscriber image_sub_;
};

#endif