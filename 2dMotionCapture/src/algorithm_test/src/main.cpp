#include "algorithm_test/analogCamera.h"
#include "algorithm_test/matplotlibcpp.h"

using namespace std;
namespace plt = matplotlibcpp;
int main(int argc, char **argv){
    AnalogCamera camera;
    camera.Generate3DPoints();
    camera.Calculate2DPoints();
    camera.SolvePnP();
   // camera.showCalibration();

    camera.GeneratePlanar3DPoints();
    camera.Calculate2DPoints();
    camera.Calculate3DPoints(); 
    //画图
    plt::figure(1);
    vector<double> n;
    vector<double> diff;
    for( int i = 0; i< camera.calculate_x_w_.size(); i++){
        diff.push_back(abs(camera.calculate_x_w_[i]-camera.real_x_w_[i]) + abs(camera.calculate_y_w_[i]-camera.real_y_w_[i]));
        n.push_back(i);
      
    }
    plt::plot(n, diff);
    plt::xlabel("Index");
    plt::ylabel("Difference");
    plt::title("Real vs. Calculated Points");  
   
    plt::show();

    return 0;
}