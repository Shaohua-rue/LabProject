#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //隐藏Qt自带的边框
    this->setWindowFlag(Qt::FramelessWindowHint);

    QImage qtImage("F:/Resources/Picture/1.BMP");
    QPixmap pixmap = QPixmap::fromImage(qtImage);
    QGraphicsPixmapItem *item = new QGraphicsPixmapItem(pixmap);
    m_scene = new QGraphicsScene(this);
    m_scene->addItem(item);

    ui->cameraImage->setScene(m_scene);


}

MainWindow::~MainWindow()
{
    delete ui;
}


