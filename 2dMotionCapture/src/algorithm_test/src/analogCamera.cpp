#include "algorithm_test/analogCamera.h"


inline float generateRandomFloat(float min, float max) {
    // 创建随机数引擎
    std::random_device rd;  // 用于获取随机种子
    std::mt19937 gen(rd()); // 使用Mersenne Twister引擎

    // 创建均匀分布
    std::uniform_real_distribution<> dis(min, max);

    // 生成随机数
    return dis(gen);
}
 AnalogCamera::AnalogCamera()
 {
    K_ = (cv:: Mat_<double> ( 3,3 ) << 520.9, 0, 325.1, 
                                                                        0, 521.0, 249.7, 
                                                                        0, 0, 1 );
    t_ = (cv:: Mat_<double> ( 3,1 ) << 100, 200, 400 );
    r_ = (cv:: Mat_<double> ( 3,1 ) << 0.5, 0.5, 0.5 );
    cv::Rodrigues ( r_, R_ );     //Rodrigues公式
 }

 void AnalogCamera::Generate3DPoints(){
    objectPoints_.clear();
    for (int x = -80; x <= 80; x += 10) {
        for(int y = -80; y <= 80; y += 10) {
            for(int z = -80; z <= 80; z += 10) {
                cv::Point3f point(x, y, z);
                objectPoints_.push_back(point);
            }
        }
    }
 }
void AnalogCamera::Calculate2DPoints() {
    imagePoints_.clear();
    for (int i = 0; i < objectPoints_.size(); i++) {
        // 将 cv::Point3f 转换为 cv::Mat
        cv::Mat point3d = (cv::Mat_<double>(4, 1) << objectPoints_[i].x, objectPoints_[i].y, objectPoints_[i].z, 1);
        
        // 计算旋转和平移后的点
        cv::Mat transformedPoint = R_ * point3d.rowRange(0, 3) + t_;
        
        // 添加齐次坐标
        transformedPoint.push_back(1.0);
        
        // 计算内参矩阵乘以变换后的点
        cv::Mat tmpPoint = K_ * transformedPoint.rowRange(0, 3);
        
        // 归一化得到图像坐标
        cv::Point2f point(tmpPoint.at<double>(0) / tmpPoint.at<double>(2), 
                          tmpPoint.at<double>(1) / tmpPoint.at<double>(2));
        
        imagePoints_.push_back(point);
    }
}
void AnalogCamera::SolvePnP(){
    cv::solvePnP(objectPoints_, imagePoints_, K_, cv::Mat(), cal_r_, cal_t_, false);
    cv::Rodrigues ( cal_r_, cal_R_ );     //Rodrigues公式
}
void AnalogCamera::showCalibration(){
    std::cout << "real R = " << R_ << std::endl;
    std::cout << "real t = " << t_ << std::endl;


    std::cout << "calculate R = " << cal_R_ << std::endl;
    std::cout << "calculate t = " << cal_t_ << std::endl;
}

void AnalogCamera::GeneratePlanar3DPoints(){
    objectPoints_.clear();
    float min = -80.0f;
    float max = 80.0f;
    for ( int n = 0; n < 300; n++ ){
        float x = generateRandomFloat(min, max);
        float y = generateRandomFloat(min, max);
        cv::Point3f point(x, y, z);
        real_x_w_.push_back(x);
        real_y_w_.push_back(y);
        objectPoints_.push_back(point);
    }

}
void AnalogCamera::Calculate3DPoints(){
    for(auto& imagePoint : imagePoints_)
    {
        float u = imagePoint.x;
        float v = imagePoint.y;
        float alpha = (u - K_.at<double>(0, 2)) / K_.at<double>(0, 0);
        float beta = (v - K_.at<double>(1, 2)) / K_.at<double>(1, 1);
        cv::Mat A = (cv::Mat_<float>(2, 2) << cal_R_.at<double>(0, 0) - alpha * cal_R_.at<double>(2, 0),  cal_R_.at<double>(0, 1) - alpha * cal_R_.at<double>(2, 1),
                                                                                  cal_R_.at<double>(1, 0) - beta * cal_R_.at<double>(2, 0),  cal_R_.at<double>(1, 1) - beta * cal_R_.at<double>(2, 1));
       cv::Mat b = (cv::Mat_<float>(2, 1) << (alpha * cal_R_.at<double>(2, 2) - cal_R_.at<double>(0, 2)) * z  + (alpha * cal_t_.at<double>(2) - cal_t_.at<double>(0)),
                                             (beta * cal_R_.at<double>(2, 2) - cal_R_.at<double>(1, 2)) * z  + (beta * cal_t_.at<double>(2) - cal_t_.at<double>(1)));
        cv::Mat x;
        cv::solve(A, b, x);
        calculate_x_w_.push_back(x.at<float>(0, 0));
        calculate_y_w_.push_back(x.at<float>(1, 0));
        
    }
}