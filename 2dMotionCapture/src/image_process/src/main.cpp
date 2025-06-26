#include  "image_process/imageProcess.h"

int main(int argc, char **argv)
{
    ros::init(argc, argv, "image_process");
    ImageProcess image_process;
    image_process.run();
    return 0;
}