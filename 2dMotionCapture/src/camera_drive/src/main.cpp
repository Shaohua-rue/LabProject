#include<iostream>
#include<ros/ros.h>       
#include "cameraDrive/cameraDrive.h"
int main(int argc, char** argv) {
    ros::init(argc,argv,"camera_drive");
    CameraDriveNode cameraDriveNode;
    cameraDriveNode.run();

    return 0;
}