#include "cameraDrive/cameraDrive.h"

CameraDriveNode::CameraDriveNode()
{
    //创建发布者、订阅者
    image_transport::ImageTransport it(nh_);
    imagePub_ = it.advertise("/camera/image_raw", 1);
    cap_.open(0);
    if (! cap_.isOpened()) {
    std::cerr<<"ERROR! Unable to open camera"<<std::endl;
    ros::shutdown();
    }
}

//运行节点的主循环(通常是一个单独的线程或者在main函数中调用)
void CameraDriveNode::run()
{
    ros::Rate loop_rate(10); // 设置循环频率
    while (ros::ok())
    {
        // 在这里可以添加发布消息、处理订阅消息等逻辑
        cap_.read(frame_);
        if(!frame_.empty()){
            sensor_msgs::ImagePtr msg = cv_bridge::CvImage(std_msgs::Header(), "bgr8", frame_).toImageMsg();
            imagePub_.publish(msg);
        }

        ros::spinOnce();   // 处理回调函数队列中的一次回调
        loop_rate.sleep(); // 按照设置的频率休眠
    }
}