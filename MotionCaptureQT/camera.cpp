#include "camera.h"
QImage cvMat2QImage(const cv::Mat &mat);
void Camera::openCamera(const int &hCamera,vector<Point2d>Corner, QImage& qtImage)
{
    // 获得该相机的特性描述
     tSdkCameraCapbility CameraInfo;
     CameraGetCapability(hCamera, &CameraInfo);//hCamera:相机句柄   &CameraInfo：指针，返回相机特性描述的结构体

    //判断是黑白相机还是彩色相机，黑白相机让ISP直接输出MONO数据，而不是扩展成R=G=B的24位灰度
    BOOL bMonoCamera = CameraInfo.sIspCapacity.bMonoSensor;
    if (bMonoCamera)
    {
      CameraSetIspOutFormat(hCamera, CAMERA_MEDIA_TYPE_MONO8);
    }

    // 相机模式切换成连续采集，设置曝光方式与时间
    CameraSetTriggerMode(hCamera, 0);//一般情况，0表示连续采集模式，1表示软件触发模式，2表示硬件触发模式
    CameraSetAeState(hCamera, FALSE);//true是自动，false是手动
    CameraSetExposureTime(hCamera, 50* 1000);// 手动曝光，曝光时间50ms

    // 让SDK内部取图线程开始工作
    CameraPlay(hCamera);//让 SDK 进入工作模式，开始接收来自相机发送的图像

   // 计算RGB buffer所需的大小，这里直接按照相机的最大分辨率来分配，分配RGB buffer，用来存放ISP输出的图像
   UINT FrameBufferSize = CameraInfo.sResolutionRange.iWidthMax * CameraInfo.sResolutionRange.iHeightMax * (bMonoCamera ? 1 : 3);
   BYTE* pFrameBuffer = (BYTE*)CameraAlignMalloc(FrameBufferSize, 16);//申请地址对齐的内存块。用于配合硬件加速算法，需要使用对齐的内存块

   while (!cameraExit)
   {
   //----------------------获取单帧图像-------------------------------------------
       tSdkFrameHead FrameHead;
       BYTE* pRawData;
       cameraStatus = CameraGetImageBuffer(hCamera, &FrameHead, &pRawData, 2000);
       if(cameraStatus == CAMERA_STATUS_SUCCESS)
       {
            cameraStatus = CameraImageProcess(hCamera, pRawData, pFrameBuffer, &FrameHead);
            if(cameraStatus == CAMERA_STATUS_SUCCESS)
            {
                if (m_sFrInfo.iWidth != FrameHead.iWidth || m_sFrInfo.iHeight != FrameHead.iHeight)
                {
                    m_sFrInfo.iWidth = FrameHead.iWidth;
                    m_sFrInfo.iHeight = FrameHead.iHeight;
                    //图像大小改变，通知重绘
                }
                cv::Mat matImage;
                matImage = cv::Mat(cvSize(FrameHead.iWidth, FrameHead.iHeight),FrameHead.uiMediaType == CAMERA_MEDIA_TYPE_MONO8 ? CV_8UC1 : CV_8UC3,pFrameBuffer);
                //对图像进行腐蚀，抑制过大角点重复检测
                Mat element = getStructuringElement(MORPH_RECT, Size(2, 2), Point(-1, -1));   //自定义核，主要更改Size(5, 5)
                erode(matImage, matImage, element);    //进行腐蚀操作

                namedWindow("MindVisionWindow",WINDOW_NORMAL);
                imshow("MindVisionWindow",matImage);
                qtImage=cvMat2QImage(matImage);


                //按键检测退出
                int keyBoardInput=waitKey(5);
                if(keyBoardInput == 'q' || keyBoardInput == 'Q' || (keyBoardInput & 255) == 27)
                {
                    cameraExit = TRUE;
                    break;
                }
            }
       }

   }
   CameraUnInit(hCamera);// 关闭相机
   CameraAlignFree(pFrameBuffer);//释放由 CameraAlignMalloc 申请地址对齐的内存块。
}
QImage cvMat2QImage(const cv::Mat &mat)
{
    QImage image;
    switch(mat.type())
    {
    case CV_8UC1:
        // QImage构造：数据，宽度，高度，每行多少字节，存储结构
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_Grayscale8);
        break;
    case CV_8UC3:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
        image = image.rgbSwapped(); // BRG转为RGB
        // Qt5.14增加了Format_BGR888
        // image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.cols * 3, QImage::Format_BGR888);
        break;
    case CV_8UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        break;
    case CV_16UC4:
        image = QImage((const unsigned char*)mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGBA64);
        image = image.rgbSwapped(); // BRG转为RGB
        break;
    }
    return image;
}
