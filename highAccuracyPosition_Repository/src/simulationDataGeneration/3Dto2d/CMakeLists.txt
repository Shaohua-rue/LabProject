cmake_minimum_required(VERSION 2.8)
project(3DTo2D)


#生成动态库.so文件，默认在build文件夹中
SET(LIBREAD3DPOINTS_SRC Read3dPoints.cpp)
ADD_LIBRARY(Read3dPoints SHARED ${LIBREAD3DPOINTS_SRC})


#生成文件到指定位置
set(LIBRARY_OUTPUT_PATH /home/shaohua/highPrecisionLocation/lib)


#添加可执行程序
add_executable(3dTo2d 3dTo2d.cpp)
#连接，将动态库链接到可执行文件
target_link_libraries(3dTo2d Read3dPoints ${OpenCV_LIBS})