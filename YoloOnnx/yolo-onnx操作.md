参考博客:https://github.com/Li-99/yolov8_onnxruntime

#-------------查看NVIDIA cuda cudann版本
nvidia-smi
dpkg -l | grep cudnn

本机配置：libcudnn8                                             8.9.5.29-1+cuda12.2

#--------------ONNX RUNTIME下载   
结合自己的cuda与cudann下载
https://github.com/microsoft/onnxruntime/releases 

这里我下载的是onnxruntime-linux-x64-cuda12-1.17.0 


#---------------CMakeLists的修改
修改target_include_directories与target_link_libraries的地址

#----------------编译
mkdir build
cd build
cmake 
make

#---运行
./build/yolov8_ort -m ./models/yolov8n.onnx -i ./Imginput -o ./Imgoutput -c ./models/coco.names -x m --gpu

报错无法链接到libcufft.so.11
将cudalib中的文件粘贴到/use/local/cuda/lib64下


