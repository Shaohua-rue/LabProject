cmake_minimum_required(VERSION 2.8)
project(externalParameterScaleRecover)




#添加可执行程序
add_executable(exterParamScaleRecover exterParamScaleRecover.cpp)
#连接，将动态库链接到可执行文件
target_link_libraries(exterParamScaleRecover Read3dPoints ${OpenCV_LIBS})

