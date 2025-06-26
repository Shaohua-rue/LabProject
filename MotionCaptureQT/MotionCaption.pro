QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    camera.cpp \
    main.cpp \
    mainwindow.cpp \
    mindvisioncamrea.cpp \
    openglwidget.cpp

HEADERS += \
    camera.h \
    common.h \
    mainwindow.h \
    mindvisioncamrea.h \
    openglwidget.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

#解决Qt输出乱码问题与cout编译问题
msvc {
QMAKE_CFLAGS += /utf-8
QMAKE_CXXFLAGS += /utf-8
}
# 屏蔽 C4828 警告
QMAKE_CXXFLAGS_WARN_ON += -wd4828

#**************************************添加opencv动态库与头文件****************************************************
win32:CONFIG(release, debug|release): LIBS += -LD:/Library_File/OpenCV/opencv3/opencv/build/x64/vc14/lib/ -lopencv_world3410
else:win32:CONFIG(debug, debug|release): LIBS += -LD:/Library_File/OpenCV/opencv3/opencv/build/x64/vc14/lib/ -lopencv_world3410d

INCLUDEPATH += D:/Library_File/OpenCV/opencv3/opencv/build/include
DEPENDPATH += D:/Library_File/OpenCV/opencv3/opencv/build/include

#***************************************添加Eigen头文件**************************************************************
INCLUDEPATH += D:/Library_File/Ceres/eigen/eigen-3.3.9/eigen-3.3.9

#****************************************添加imgui头文件***************************************************************
INCLUDEPATH +=C:/Users/WangShaoHua/Desktop/MotionCaptureQt/MotionCaption/include
#*************************************添加MindVision相机动态库与头文件**************************************************
unix|win32: LIBS += -LD:/Library_File/Mindvision/lib/ -lMVCAMSDK_X64

INCLUDEPATH += D:/Library_File/Mindvision/include
DEPENDPATH += D:/Library_File/Mindvision/include

RESOURCES += \
    motionCapture.qrc
