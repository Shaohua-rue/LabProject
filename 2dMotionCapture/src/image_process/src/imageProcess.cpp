#include "image_process/imageProcess.h"

ImageProcess::ImageProcess()
{
    cout << "ImageProcess" << endl;
    image_transport::ImageTransport it(nh_);
    image_sub_= it.subscribe("/camera/image_raw", 1, std::bind(&ImageProcess::imageCallback, this, std::placeholders::_1));
}
void ImageProcess::run(){
     ros::Rate loop_rate(10); // 设置循环频率
    while (ros::ok())
    {
        ros::spinOnce();   // 处理回调函数队列中的一次回调
        loop_rate.sleep(); // 按照设置的频率休眠
    }
}
 void ImageProcess::imageCallback(const sensor_msgs::ImageConstPtr& msg){
    try {
            image_ = cv_bridge::toCvShare(msg, "bgr8")->image;
            cv::imshow("view", image_);
            QRCode(image_);
            cv::waitKey(1);
            
    }
    catch (cv_bridge::Exception& e) {
        ROS_ERROR("Could not convert from '%s' to 'bgr8'.", msg->encoding.c_str());
    }
 }

 void ImageProcess::QRCode(cv::Mat& image)
{
    vector<std::string> informations;
    vector<Point2d> points;
    cv::Mat gray;
    bool isQRcode=false;
    informations.clear();
    points.clear();
    cvtColor(image, gray, COLOR_BGR2GRAY);
    cv::QRCodeDetector qrCodeDetector;
    isQRcode = qrCodeDetector.detectAndDecodeMulti(gray, informations, points);  
    if (isQRcode==true) 
    {
        int p = 0;
        int m =3;
        for(int K=0;K< informations.size();K++)
        {
            for (int i = p; i < (p+4); i++)
            {
                if (i == m) 
                {
                    line(image, points[p], points[p + 3], Scalar(0, 0, 255, 255), 2, 8);
                }
                else
                {
                    line(image, points[i], points[i + 1], Scalar(0, 0, 255, 255), 2, 8);
                }
                putText(image, to_string(i + 1), points[i], 2, 1, Scalar(0, 0, 255, 255), 4);
               
                cv::imshow("image",image);
                cv::waitKey(1);
            }
            p = p + 4;
            m = m + 4;      
        }
    }

}