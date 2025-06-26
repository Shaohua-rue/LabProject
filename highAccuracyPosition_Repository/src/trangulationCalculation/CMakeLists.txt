cmake_minimum_required(VERSION 2.8)
project(trangulationCalculation)


#生成动态库.so文件，默认在build文件夹中
SET(LIBHOMOREAD2DAND2D_SRC homoRead2dAnd2d.cpp)
ADD_LIBRARY(homoRead2dAnd2d SHARED ${LIBHOMOREAD2DAND2D_SRC})


#生成文件到指定位置
set(LIBRARY_OUTPUT_PATH /home/shaohua/highPrecisionLocation/lib)


#添加可执行程序
add_executable(trangulation trangulation.cpp)
#连接，将动态库链接到可执行文件
target_link_libraries(trangulation homoRead2dAnd2d ${OpenCV_LIBS})
