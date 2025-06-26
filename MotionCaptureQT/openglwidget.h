#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include<vector>
#include<iostream>

//qtOpenGL核心头文件
#include<QOpenGLWidget>
#include<QOpenGLFunctions_3_3_Core>


#include<QOpenGLShaderProgram>
#include<QOpenGLTexture>
#include<tool/gui.h>
#include<tool/cameraClass.h>
#include<tool/read_data.h>

#include <QMouseEvent>

#include<opencv2/opencv.hpp>
#define PI 3.14159265358979323846f

using namespace std;
using namespace cv;


//**************************OpenGL类的构建*********************************
class OpenGLWidget: public QOpenGLWidget, QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    OpenGLWidget(QWidget* parent);
    ~OpenGLWidget();
protected:

    //**********************QOpenGLWidget提供的三个（可重载的）虚函数用于实现典型的OpenGL任务*****
    //注意：如果要从paintGL()以外的位置触发重新绘制，则应该调用widget的update()函数来安排更新

    //----初始化---
    void initializeGL(); //设置OpenGL的资源与状态。第一次调用resizeGL()/paintGL()之前调用一次

    //----循环渲染---
    void paintGL(); //渲染OpenGL场景。widget需要更新时调用

    //----视窗大小调整---
    void resizeGL(int width, int height); //设置OpenGL窗口、投影等。widget调整大小(或者首次显示时调用)

    //********************************鼠标与按键事件检测***************************************
    //注意：调用QOpenGLWidget重载的三个虚函数时，widget的OpenGL呈现的上下文会变为当前；
    //如果需要从其他位置调用标准的OpenGL API函数，则必须先调用makeCurrent();
    void mouseMoveEvent(QMouseEvent* event);
    void mousePressEvent(QMouseEvent* event);
    void mouseReleaseEvent(QMouseEvent* event);
    void wheelEvent(QWheelEvent* event);

    private:
        QOpenGLShaderProgram shaderProgram_mesh;
        QOpenGLShaderProgram shaderProgram_axis;
        QOpenGLShaderProgram shaderProgram_point;
        QOpenGLShaderProgram shaderProgram_camera;
        readData readCsvData;

        unsigned int VBO_MeshLine;
        unsigned int VAO_MeshLine;

        unsigned int VBO_Axis;
        unsigned int VAO_Axis;

        unsigned int VBO_Point;
        unsigned int VAO_Point;

        unsigned int VBO_Camera;
        unsigned int VAO_Camera;




        unsigned int vertex_count;
        unsigned int point_count;
        std::vector<Point3f>Points_3f;
        std::vector<float> pointData;

        float x_rotate;
        float z_rotate;
        float x_trans;
        float y_trans;
        float zoom;

        bool first_mouse;
        bool leftMousePress;
        bool RightMousePress;


        //-----------------------OpenGL网格,坐标轴，点云的绘制----------------------------------
        unsigned int draw_meshline(float size, int count);
        void draw_cooraxis(float length);
        void draw_Camera();
        unsigned int draw_pointdata(std::vector<float>& point_vertexs);
        vector<float> drawSphereData(float radius, vector<Point3f> points_3f);
};

#endif // OpenGLWidget_H
