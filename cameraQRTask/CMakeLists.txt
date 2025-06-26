cmake_minimum_required(VERSION 3.10)
project(CameraNode)

set(CMAKE_CXX_STANDARD 14)
set(camera_node camera_node)
set(OpenCV_DIR /usr/local/opencv4.5.5/lib/cmake/opencv4)
# 查找OpenCV库
find_package(OpenCV REQUIRED)

include_directories(
    ${OpenCV_INCLUDE_DIRS}  # 包含OpenCV的头文件
    ${CMAKE_SOURCE_DIR}/include
    # /lib/libMVSDK.so
    # /lib
)
link_directories(/lib)
add_executable(
    ${camera_node}
    src/main.cpp
    src/Camera.cpp
)
# 链接OpenCV库
target_link_libraries(${camera_node} 
${OpenCV_LIBS} 
/lib/libMVSDK.so
-lpthread)
# 设置输出的可执行文件目录到CMakeLists.txt同级目录下的install文件夹
set_target_properties(${camera_node} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/install)

install(TARGETS  ${camera_node} DESTINATION  ${CMAKE_SOURCE_DIR}/install)
