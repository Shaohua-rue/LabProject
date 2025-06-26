#ifndef CAMERA_DRIVE_H
#define CAMERA_DRIVE_H
#include <iostream>
#include <ros/ros.h>  
#include <std_msgs/String.h>  
#include <image_transport/image_transport.h>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>
using namespace std;

class CameraDriveNode
{
public:
  CameraDriveNode();
   void run();

private:
    ros::NodeHandle nh_;     // NodeHandle对象，用于与ROS系统交互  
    image_transport::Publisher imagePub_;
     
    cv::VideoCapture cap_;
    cv::Mat frame_;
};

#endif