cmake_minimum_required(VERSION 2.8)

project(highPercisionLocation)

# 寻找工程自定义头文件,意思是当前可执行程序所使用的头文件在include中
include_directories(${PROJECT_SOURCE_DIR}/include)

# 寻找OpenCV库
find_package(OpenCV REQUIRED)
# 添加头文件
include_directories(${OpenCV_INCLUDE_DIRS})

# 添加Eigen头文件
include_directories("/usr/include/eigen3")

add_subdirectory(src/simulationDataGeneration/3Dto2d bin/3Dto2d)
add_subdirectory(src/simulationDataGeneration/generate3DPoint bin/generate3DPoint)
add_subdirectory(src/cornerExtractionAlgorithm bin/cornerExtractionAlgorithm)
add_subdirectory(src/foundmentalMatrixCalculate/OpenCVFundamental bin/OpenCVFundamental)
add_subdirectory(src/foundmentalMatrixCalculate/fundamentalCalculate bin/fundamentalCalculate)
add_subdirectory(src/externalParameterCalculation bin/externalParameterCalculation)
add_subdirectory(src/trangulationCalculation bin/trangulationCalculation)
add_subdirectory(src/externalParameterScaleRecover bin/externalParameterScaleRecover)
