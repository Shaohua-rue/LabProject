cmake_minimum_required(VERSION 2.8)
project(generate3DPoint)

#添加可执行程序
add_executable(simple3D simple3D.cpp)
add_executable(calibrationRod3d calibrationRod3d.cpp)
#连接，将动态库链接到可执行文件
target_link_libraries(simple3D ${OpenCV_LIBS})
target_link_libraries(calibrationRod3d ${OpenCV_LIBS})
