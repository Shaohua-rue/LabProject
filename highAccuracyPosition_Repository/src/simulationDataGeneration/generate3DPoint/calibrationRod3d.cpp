#include <iostream>
#include <cmath>
#include <random>
#include <fstream>
#include <vector>
#include <fstream>
// 定义三点坐标的结构体
struct Point {
   double x;
   double y;
   double z;
};

// 计算两点间的距离
double distance(const Point& p1, const Point& p2) {
   double dx = p2.x - p1.x;
   double dy = p2.y - p1.y;
   double dz = p2.z - p1.z;
   return std::sqrt(dx * dx + dy * dy + dz * dz);
}

// 随机生成在圆内的点
Point generateRandomPointInCircle(const Point& center, double radius) {
   std::random_device rd;
   std::mt19937 gen(rd());
   std::uniform_real_distribution<double> dis_theta(0.0, 2.0 * 3.14);
   std::uniform_real_distribution<double> dis_phi(0.0, 3.14);
   std::uniform_real_distribution<double> dis_radius(0.0, radius);

   double theta = dis_theta(gen);
   double phi = dis_phi(gen);
   double r = dis_radius(gen);

   Point p;
   p.x = center.x + r * std::sin(phi) * std::cos(theta);
   p.y = center.y + r * std::sin(phi) * std::sin(theta);
   p.z = center.z + r * std::cos(phi);

   return p;
}

// 计算 A、B、C 三点的坐标
void calculatePoints(const Point& center, double radius,double ac, Point& a, Point& b, Point& c) {
   // 随机生成挥舞点 A
   a = generateRandomPointInCircle(center, radius);


   // 计算 C 点的坐标
   double acLength = distance(a, c);
   double scaleFactorAC = ac / acLength;
   c.x = a.x + scaleFactorAC * (c.x - a.x);
   c.y = a.y + scaleFactorAC * (c.y - a.y);
   c.z = a.z + scaleFactorAC * (c.z - a.z);
 
   // 计算 B 点的坐标
   b.x = 0.4 * c.x + 0.6 * a.x;
   b.y = 0.4 * c.y + 0.6 * a.y;
   b.z = 0.4 * c.z + 0.6 * a.z;
}

int main() {
   // 设置随机种子
   std::srand(time(0));

   // 定义圆心和半径
   Point center = { 500, 500, 500 };
   double radius = 300;

   // 定义约束条件
   double ac = 50;

   const int numSwings = 300;

   std::vector<Point> points;

   for (int i = 0; i < numSwings; ++i) {
       Point a, b, c;
       calculatePoints(center, radius,  ac, a, b, c);
       std::cout << "A: (" << a.x << ", " << a.y << ", " << a.z << ")\n";
       std::cout << "B: (" << b.x << ", " << b.y << ", " << b.z << ")\n";
       std::cout << "C: (" << c.x << ", " << c.y << ", " << c.z << ")\n";
       points.push_back(a);
       points.push_back(b);
       points.push_back(c);
   }

   // 打开文件进行写入
   std::ofstream outputFile("/home/shaohua/highPrecisionLocation/data/Output/calibrationRod3dPoints.txt");
   if (outputFile.is_open()) {
       // 将点保存到文件中
       for (const Point& p : points) {
           outputFile << p.x << " " << p.y << " " << p.z << "\n";
       }

       outputFile.close();
       std::cout << "Points saved to 'calibrationRod3dPoints.txt'.\n";
   }
   else {
       std::cout << "Failed to open file for writing.\n";
   }


   return 0;
}

