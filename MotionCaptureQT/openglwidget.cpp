#include "openglwidget.h"

//*****************************构造函数***************************
OpenGLWidget::OpenGLWidget(QWidget* parent): QOpenGLWidget(parent)
{

    //设置OpenGL的Surface格式,设置4倍抗锯齿
    QSurfaceFormat format;
    format.setSamples(4);
    setFormat(format);

    //模型矩阵model的初始参数设置
    x_rotate = -40.0;
    z_rotate = 90.0;
    x_trans = 0.0;
    y_trans = 0.0;

    //投影矩阵projection的视角(fov),单位为 °
    zoom = 45.0;

    //鼠标首次移动到OpenGL窗口和鼠标按下标志
    first_mouse = true;
    leftMousePress = false;
    RightMousePress = false;

}
//****************************析构函数**************************
OpenGLWidget::~OpenGLWidget()
{
    makeCurrent();

    //释放VAO与VBO
    glDeleteBuffers(1, &VBO_MeshLine);
    glDeleteVertexArrays(1, &VAO_MeshLine);

    glDeleteBuffers(1, &VBO_Axis);
    glDeleteVertexArrays(1, &VAO_Axis);

    glDeleteBuffers(1, &VBO_Camera);
    glDeleteVertexArrays(1, &VAO_Camera);

    glDeleteBuffers(1, &VBO_Point);
    glDeleteVertexArrays(1, &VAO_Point);


    //释放着色器程序
    shaderProgram_mesh.release();
    shaderProgram_axis.release();
    shaderProgram_point.release();
    shaderProgram_camera.release();
    doneCurrent();

}

//****************************QOpenGL的初始化函数****************************
void OpenGLWidget::initializeGL()
{
  //初始化OpenGL函数
  initializeOpenGLFunctions();  //将Qt里的函数指针指向显卡的函数

 // 开启深度测试
  glEnable(GL_DEPTH_TEST);



  //----链接着色器程序----------------------------------------
  // 链接网格着色器
  shaderProgram_mesh.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resource/shader/shader_mesh.vs");
  shaderProgram_mesh.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resource/shader/shader_mesh.fs");
  shaderProgram_mesh.link();

  // 链接坐标轴着色器
  shaderProgram_axis.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resource/shader/shader_axis.vs");
  shaderProgram_axis.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resource/shader/shader_axis.fs");
  shaderProgram_axis.link();

  // 链接点云着色器
  shaderProgram_point.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resource/shader/shader_point.vs");
  shaderProgram_point.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resource/shader/shader_point.fs");
  shaderProgram_point.link();

  //链接相机模型着色器
  shaderProgram_camera.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/resource/shader/shader_camera.vs");
  shaderProgram_camera.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/resource/shader/shader_camera.fs");
  shaderProgram_camera.link();

  //----进行网格与坐标轴生成以及VAO与VBO的绑定---------------------
  vertex_count = draw_meshline(0.5, 15);//两个参数分别为网格尺寸与网格数量
  draw_cooraxis(0.5);//参数为坐标轴大小
  draw_Camera();
}
//****************************QOpenGL的渲染函数****************************
void OpenGLWidget::paintGL()
{

    //---背景色的设置与使用
    glClearColor(0.21f, 0.30f, 0.27f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //--------点云数据的读取与绑定----------
    //设置点云数据

    Points_3f.clear();
    for (float i=0;i<3;i++) {
        Point3f m{-i,-i,1.0f};
        Points_3f.push_back(m);
    }
     pointData=drawSphereData(1.0f,Points_3f);
     point_count = draw_pointdata(pointData);

     pointData.clear();

    //---3D视图中rojection, view, model的设置与使用
    QMatrix4x4 projection, view, model,Rotate;
    QVector3D Position(-3.8,-3.8,3.0);
    projection.perspective(zoom, (float)width() / (float)height(), 1.0f, 100.0f);

    view.lookAt(QVector3D(0.0, 0.0, 10.0), QVector3D(0.0, 0.0, 1.0), QVector3D(0.0, 1.0, 0.0));

    model.translate(x_trans, y_trans, 0.0);
    model.rotate(x_rotate, 1.0, 0.0, 0.0);
    model.rotate(z_rotate, 0.0, 0.0, 1.0);

    Rotate.rotate(45,0.0, 1.0, 1.0);

    shaderProgram_mesh.bind();
    shaderProgram_mesh.setUniformValue("projection", projection);
    shaderProgram_mesh.setUniformValue("view", view);
    shaderProgram_mesh.setUniformValue("model", model);

    shaderProgram_axis.bind();
    shaderProgram_axis.setUniformValue("projection", projection);
    shaderProgram_axis.setUniformValue("view", view);
    shaderProgram_axis.setUniformValue("model", model);

    shaderProgram_point.bind();
    shaderProgram_point.setUniformValue("projection", projection);
    shaderProgram_point.setUniformValue("view", view);
    shaderProgram_point.setUniformValue("model", model);

    shaderProgram_camera.bind();
    shaderProgram_camera.setUniformValue("projection", projection);
    shaderProgram_camera.setUniformValue("view", view);
    shaderProgram_camera.setUniformValue("model", model);
    shaderProgram_camera.setUniformValue("Rotate", Rotate);
    shaderProgram_camera.setUniformValue("Position", Position);


    //---使用网格着色器程序绘制网格顶点数据VAO_MeshLine
    shaderProgram_mesh.bind();
    glBindVertexArray(VAO_MeshLine);
    glLineWidth(1.0f);
    glDrawArrays(GL_LINES,0, vertex_count);

    //---使用坐标轴着色器程序绘制坐标轴顶点数据VAO_Axis
    shaderProgram_axis.bind();
    glBindVertexArray(VAO_Axis);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 6);

    //---使用相机着色器程序绘制坐标轴顶点数据VAO_Axis
    shaderProgram_camera.bind();
    glBindVertexArray(VAO_Camera);
    glDrawArrays(GL_TRIANGLES, 0, 72);
    //---使用点云着色器程序绘制坐标轴顶点数据VAO_Point
    shaderProgram_point.bind();
    glBindVertexArray(VAO_Point);
    glPointSize(1.0f);
    glDrawArrays(GL_POINTS, 0, point_count);
}
//****************************QOpenGL的视窗初始化与其大小改变后的重绘函数****************************
void OpenGLWidget::resizeGL(int width, int height)
{
    glViewport(0, 0, width, height);
}

//************************************鼠标按下与释放检测*****************************************
//用于提供给鼠标移动对应事件的标志位
void OpenGLWidget:: mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        first_mouse = true;
        leftMousePress = true;
    }

    else if (event->button() == Qt::RightButton)
    {
        first_mouse = true;
        RightMousePress = true;
    }

}
void OpenGLWidget:: mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        leftMousePress = false;
    }
    else if (event->button() == Qt::MidButton)
    {
        RightMousePress = false;
    }

}

//************************************鼠标移动事件检测********************************************
//模型旋转与平移的设置
void OpenGLWidget:: mouseMoveEvent(QMouseEvent* event)
{

  static QPoint lastPos;
  auto currentPos = event->pos();
  QPoint deltaPos;

  //---模型旋转设置
  //首次鼠标左键按下时记录当前鼠标所在的位置，与下一时刻鼠标左键按下后位置做对比，通过坐标差值设置模型旋转参数
  if (leftMousePress == true)
  {
      if (first_mouse == true)
      {
          lastPos = currentPos;
          first_mouse = false;
      }

      deltaPos = currentPos - lastPos;
      lastPos = currentPos;

      x_rotate = x_rotate + 0.3 * deltaPos.y();
      z_rotate = z_rotate + 0.3 * deltaPos.x();

      if (x_rotate > 30.0f)
          x_rotate = 30.0f;
      if (x_rotate < -120.0f)
          x_rotate = -120.0f;
  }
  //---模型平移设置
  //首次鼠标右键按下时记录当前鼠标所在的位置，与下一时刻鼠标左键按下后位置做对比，通过坐标差值设置模型旋转参数
  else if (RightMousePress == true)
  {
      if (first_mouse == true)
      {
          lastPos = currentPos;
          first_mouse = false;
      }

      deltaPos = currentPos - lastPos;
      lastPos = currentPos;

      x_trans = x_trans + 0.01 * deltaPos.x();
      y_trans = y_trans - 0.01 * deltaPos.y();
  }
  update();

}
//************************************鼠标滚轮事件检测********************************************
//设置视角的大小
void OpenGLWidget:: wheelEvent(QWheelEvent* event)
{
    auto scroll_offest = event->angleDelta().y() / 120;
    zoom = zoom - (float)scroll_offest;

    if (zoom < 10.0f)
    {
        zoom = 10.0f;
    }

    if (zoom > 80.0f)
    {
        zoom = 80.0f;
    }

    update();
}

//************************************生成网格顶点数据与VAO绑定********************************************
unsigned int OpenGLWidget::draw_meshline(float size, int count)
{

   std::vector<float> mesh_vertexs;     //存储网格线的顶点坐标
   unsigned int vertex_count = 0;       //生成的顶点数量

   float start = count * (size / 2);    //通过输入的size和count计算每个网格线的起始位置
   float posX = start, posZ = start;

   for (int i = 0; i <= count; ++i)
   {
       mesh_vertexs.push_back(posX);
       mesh_vertexs.push_back(start);
       mesh_vertexs.push_back(0);

       mesh_vertexs.push_back(posX);
       mesh_vertexs.push_back(-start);
       mesh_vertexs.push_back(0);

       mesh_vertexs.push_back(start);
       mesh_vertexs.push_back(posZ);
       mesh_vertexs.push_back(0);

       mesh_vertexs.push_back(-start);
       mesh_vertexs.push_back(posZ);
       mesh_vertexs.push_back(0);

       posX = posX - size;
       posZ = posZ - size;
   }

       glGenVertexArrays(1, &VAO_MeshLine);
       glGenBuffers(1, &VBO_MeshLine);

       glBindVertexArray(VAO_MeshLine);

       glBindBuffer(GL_ARRAY_BUFFER, VBO_MeshLine);
       glBufferData(GL_ARRAY_BUFFER, mesh_vertexs.size() * sizeof(float), &mesh_vertexs[0], GL_STATIC_DRAW);

       glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
       glEnableVertexAttribArray(0);

       glBindBuffer(GL_ARRAY_BUFFER, 0);

       glBindVertexArray(0);

       vertex_count = mesh_vertexs.size() / 3;

       return vertex_count;
}

//************************************生成坐标轴顶点数据与VAO绑定********************************************
//---定义红色为X轴，绿色为Y轴，蓝色为Z轴
void OpenGLWidget::draw_cooraxis(float length)
{
    std::vector<float> axis_vertexs(36, 0.0);
    axis_vertexs[3] = 1.0;
    axis_vertexs[9] = 1.0;
    axis_vertexs[16] = 1.0;
    axis_vertexs[22] = 1.0;
    axis_vertexs[29] = 1.0;
    axis_vertexs[35] = 1.0;

    axis_vertexs[6] = length;
    axis_vertexs[19] = length;
    axis_vertexs[32] = length;

    glGenVertexArrays(1, &VAO_Axis);
    glGenBuffers(1, &VBO_Axis);

    glBindVertexArray(VAO_Axis);

    glBindBuffer(GL_ARRAY_BUFFER, VBO_Axis);
    glBufferData(GL_ARRAY_BUFFER, axis_vertexs.size() * sizeof(float), &axis_vertexs[0], GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);
}
//*******************************相机模型数据的生成与VAO绑定**********************************************************
void OpenGLWidget::draw_Camera()
{
    float cameraData[] = {
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

//          0.5f,  0.5f,  0.5f,
//          0.5f,  0.5f, -0.5f,
//          0.5f, -0.5f, -0.5f,
//          0.5f, -0.5f, -0.5f,
//          0.5f, -0.5f,  0.5f,
//          0.5f,  0.5f,  0.5f,

        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f,  0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,

        1.0f,  -1.0f,  1.0f,
        1.0f,  -1.0f, -1.0f,
        1.0f,   1.0f, -1.0f,
        1.0f,   1.0f, -1.0f,
        1.0f,   1.0f,  1.0f,
        1.0f,  -1.0f,  1.0f,

        1.0f,  -1.0f,  1.0f,
        0.5f,  -0.5f,  0.5f,
        1.0f,   0.0f,  1.0f,
        0.5f,  -0.5f,  0.5f,
        0.5f,   0.5f,  0.5f,
        1.0f,   0.0f,  1.0f,
        1.0f,   0.0f,  1.0f,
        0.5f,   0.5f,  0.5f,
        1.0f,   1.0f,  1.0f,

        1.0f,   1.0f,  1.0f,
        0.5f,   0.5f,  0.5f,
        1.0f,   1.0f,  0.0f,
        0.5f,   0.5f,  0.5f,
        1.0f,   1.0f,  0.0f,
        0.5f,   0.5f,  -0.5f,
        0.5f,   0.5f,  -0.5f,
        1.0f,   1.0f,  -1.0f,
        1.0f,   1.0f,   0.0f,

        1.0f,   1.0f,  -1.0f,
        0.5f,   0.5f,  -0.5f,
        1.0f,   0.0f,  -1.0f,
        0.5f,   0.5f,  -0.5f,
        0.5f,  -0.5f,  -0.5f,
        1.0f,   0.0f,  -1.0f,
        1.0f,  -1.0f,  -1.0f,
        1.0f,   0.0f,  -1.0f,
        0.5f,  -0.5f,  -0.5f,

        1.0f,  -1.0f,   1.0f,
        0.5f,  -0.5f,   0.5f,
        1.0f,  -1.0f,   0.0f,

        1.0f,  -1.0f,   0.0f,
        0.5f,  -0.5f,  -0.5f,
        0.5f,  -0.5f,   0.5f,

        1.0f,  -1.0f,  -1.0f,
        1.0f,  -1.0f,   0.0f,
        0.5f,  -0.5f,  -0.5f
    };
    glGenVertexArrays(1, &VAO_Camera);
    glGenBuffers(1, &VBO_Camera);

    glBindVertexArray(VAO_Camera);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Camera);

    //将顶点数据绑定到默认的缓冲中
    glBufferData(GL_ARRAY_BUFFER, sizeof(cameraData), cameraData, GL_STATIC_DRAW);


    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

}
//*******************************点云数据的VAO绑定**********************************************************
unsigned int OpenGLWidget::draw_pointdata(std::vector<float> &point_vertexs)
{
    unsigned point_count = 0;

    glGenVertexArrays(1, &VAO_Point);
    glGenBuffers(1, &VBO_Point);

    glBindVertexArray(VAO_Point);
    glBindBuffer(GL_ARRAY_BUFFER, VBO_Point);

    //将顶点数据绑定到默认的缓冲中
    glBufferData(GL_ARRAY_BUFFER, point_vertexs.size() * sizeof(float), &point_vertexs[0], GL_STATIC_DRAW);


    //设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(0);

    //计算点云数量
    point_count = point_vertexs.size()/3;

    return point_count;

}

//***************************************将三维点扩充为以该点为圆心的球面点集****************************
vector<float> OpenGLWidget::drawSphereData(float radius, vector<Point3f> points_3f)
{
        vector<float>Points;
        Points.clear();

        //将球横纵划分成20*20的网格
        const int Y_SEGMENTS = 20;
        const int X_SEGMENTS = 20;

        for (int k=0;k<points_3f.size();k++)
        {
            //生成球的顶点
            for (int y=0;y<Y_SEGMENTS;y++)
            {

                for (int x=0;x<X_SEGMENTS;x++)
                {
                    float xSegment = (float)x/(float)X_SEGMENTS;
                    float ySegment = (float)y/(float)Y_SEGMENTS;

                    float xPos = (std::cos(xSegment*2.0f*PI)*std::sin(ySegment*PI)/100.0f)*radius+points_3f[k].x;
                    float yPos = (std::cos(ySegment*PI)/100.0f)*radius+points_3f[k].y;
                    float zPos =  (std::sin(xSegment*2.0f*PI)*std::sin(ySegment*PI)/100.0f)*radius+points_3f[k].z;
                    Points.push_back(xPos);
                    Points.push_back(yPos);
                    Points.push_back(zPos);
                }
            }
         }
        return Points;
}
