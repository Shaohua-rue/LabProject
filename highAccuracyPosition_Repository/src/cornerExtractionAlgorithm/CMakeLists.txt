cmake_minimum_required(VERSION 2.8)
project(cornerExtractionAlgorithm)

#添加可执行程序
add_executable(HarrisAndSubpixel HarrisAndSubpixel.cpp)
add_executable(caliRodCorner_Harris caliRodCorner_Harris.cpp)
#连接，将动态库链接到可执行文件
target_link_libraries(HarrisAndSubpixel ${OpenCV_LIBS})
target_link_libraries(caliRodCorner_Harris ${OpenCV_LIBS})
