/**
 * 通信的服务端,rosServerClient的调用中心
 * 
 * create by shaohua 25/5/13
 * **/
#include "ros/ros.h"
#include "ros_server/observer.hpp"
#include "ros_server/server.hpp"


int main(int argc, char *argv[])
{
    //设置ros编码格式
    setlocale(LC_ALL,"");

    //初始化ros环境并创建句柄
    ros::init(argc,argv,"Login_Client");
    std::shared_ptr<ros::NodeHandle> nhPtr = std::make_shared<ros::NodeHandle>();

    //初始化网络服务
    Service server("192.168.6.130",1200);

    //创建客户端请求
    std::shared_ptr<AbstractObserver> serverLogin = std::make_shared<ServerA>(nhPtr);   //登陆请求
    std::shared_ptr<AbstractObserver> serverr = std::make_shared<ServerB>(nhPtr);   //注册请求

    //注册请求到网络服务中
    server.RegisterObserve(serverLogin);
    server.RegisterObserve(serverr);

    //开启网络服务
    server.Run();

    return 0;
}
