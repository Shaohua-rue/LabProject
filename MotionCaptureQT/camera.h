#ifndef CAMERA_H
#define CAMERA_H

//用于在C++和C语言中引入 Windows 操作系统的头文件是CameraApi依赖头文件，未包含则会报错
#include <Windows.h>
#include<CameraApi.h>
#include <QtGui>
#include "common.h"
class Camera
{
public:
   void Camera::openCamera(const int &hCamera,vector<Point2d>Corner, QImage& qtImage);

private:
    CameraSdkStatus cameraStatus;
    bool cameraExit = false;		//用来通知图像抓取线程结束
    tSdkFrameHead m_sFrInfo;		//用于保存当前图像帧的帧头信息

};

#endif // CAMERA_H
