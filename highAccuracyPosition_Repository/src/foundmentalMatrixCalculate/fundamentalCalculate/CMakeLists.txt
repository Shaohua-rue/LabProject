cmake_minimum_required(VERSION 2.8)
project(fundamentalCalculate)

#添加可执行程序
add_executable(normlizationMutiPoint normlizationMutiPoint.cpp)
add_executable(normlizationEightPoint normlizationEightPoint.cpp)
#连接，将动态库链接到可执行文件
target_link_libraries(normlizationMutiPoint Read2dAnd2d ${OpenCV_LIBS})
target_link_libraries(normlizationEightPoint Read2dAnd2d ${OpenCV_LIBS})
