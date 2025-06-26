cmake_minimum_required(VERSION 2.8)
project(externalParameterCalculation)

#添加可执行程序
add_executable(OpenCVExternalPara OpenCVExternalPara.cpp)
add_executable(externalParmeter OpenCVExternalPara.cpp)
#连接，将动态库链接到可执行文件
target_link_libraries(OpenCVExternalPara  ${OpenCV_LIBS})
target_link_libraries(externalParmeter  ${OpenCV_LIBS})
