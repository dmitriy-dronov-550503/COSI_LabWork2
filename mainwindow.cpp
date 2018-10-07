#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <iostream>
using namespace std;

// read an image
cv::Mat image;
cv::Mat tempImage;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //QWidget::showMaximized();

    loadImage();

    printImage();

    connect(ui->minValueBox, SIGNAL(valueChanged(int)), ui->minBinSlider, SLOT(setValue(int)));
    connect(ui->minBinSlider, SIGNAL(valueChanged(int)), ui->minValueBox, SLOT(setValue(int)));
    connect(ui->maxValueBox, SIGNAL(valueChanged(int)), ui->maxBinSlider, SLOT(setValue(int)));
    connect(ui->maxBinSlider, SIGNAL(valueChanged(int)), ui->maxValueBox, SLOT(setValue(int)));

    // create image window named "My Image"
    //cv::namedWindow("My Image");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::loadImage()
{
    image = cv::imread("/Users/mac/QtProjects/COSI_LabWork2/project/LabWork2/images/P0001464.jpg", CV_LOAD_IMAGE_GRAYSCALE);
    if (image.empty())
    {
        cout << "Cannot load image!" << endl;
    }
}

void MainWindow::printImage()
{
    QPixmap p = QPixmap::fromImage(QImage(image.data, image.cols, image.rows, image.step, QImage::Format_Grayscale8));
    int w = ui->label->width();
    int h = ui->label->height();
    ui->label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
}

QVector<QVector<double>> makeMatrix(string str)
{
    QVector<QVector<double>> H;
    H.push_back(QVector<double>());
    string num = "";
    int count = 0;

    for (int i = 0; i < str.size(); i++)
    {
        if (str[i] == '.' || (str[i] >= '0' && str[i] <= '9')) {
            num += str[i];
        }
        else if (str[i] == '\n' || str[i] == ',')
        {
            if (!num.empty())
            {
                double number = QString(num.c_str()).toDouble();
                H[count].push_back(number);
                num.clear();
            }
            if (str[i] == '\n')
            {
                H.push_back(QVector<double>());
                count++;
            }
        }
    }

    if (!num.empty())
    {
        double number = QString(num.c_str()).toDouble();
        H[count].push_back(number);
        num.clear();
    }

    return H;
}

void MainWindow::lowFilter()
{
    QString text = ui->textEdit->toPlainText();

    QVector<QVector<double>> H = makeMatrix(text.toUtf8().constData());

    int height = H.size();
    if (height < 1) return;
    int width = H[0].size();
    for(int i = 1; i < H.size(); i++)
    {
        if (H[i].size() < width)
        {
            width = H[i].size();
        }
    }

    cv::Mat newImage = image.clone();

    uchar* in = image.data;
    uchar* out = newImage.data;

    int S = image.cols;

    for (int y = 0 ; y < image.rows - (height - 1); y++)
    {
        for(int x = 0; x < image.cols - (width - 1); x++)
        {
            int result = 0;

            for (int i = 0; i < height; i++)
            {
                for (int j = 0; j < width; j++)
                {
                    result += H[i][j] * in[(x+i) + (y+j)*S];
                }
            }

            double divider = ui->dividerLine->text().toDouble();
            out[(x+height/2) + (y+width/2)*S] = result / divider;
        }
    }

    QPixmap p = QPixmap::fromImage(QImage(newImage.data, newImage.cols, newImage.rows, newImage.step, QImage::Format_Grayscale8));
    int w = ui->label->width();
    int h = ui->label->height();
    ui->label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));

    image = newImage;
}

void MainWindow::processImage()
{
    tempImage = image.clone();

    uchar* img = tempImage.data;

    for (int i = 0 ; i < image.rows * image.cols; i++)
    {
        if ((img[i] <= ui->minBinSlider->value()) || (img[i] >= ui->maxBinSlider->value()))
        {
            img[i] = 0;
        }
        else
        {
            img[i] = 255;
        }
    }

    QPixmap p = QPixmap::fromImage(QImage(tempImage.data, tempImage.cols, tempImage.rows, tempImage.step, QImage::Format_Grayscale8));
    int w = ui->label->width();
    int h = ui->label->height();
    ui->label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
}

void MainWindow::on_minBinSlider_valueChanged(int value)
{
    processImage();
}

void MainWindow::on_maxBinSlider_valueChanged(int value)
{
    processImage();
}


// show the image on window
//cv::resize(newImage, newImage, cv::Size(), 0.75, 0.75);
//cv::imshow("My Image", newImage);

//cv::cvtColor(newImage, newImage, CV_BGR2RGB);

void MainWindow::on_pushButton_clicked()
{
    lowFilter();
}

void MainWindow::on_pushButton_2_clicked()
{
    loadImage();
    printImage();
}

void MainWindow::on_pushButton_3_clicked()
{
    image = tempImage;
    printImage();
}

void MainWindow::on_pushButton_4_clicked()
{
    tempImage = image.clone();

    uchar* img = tempImage.data;

    QPixmap p = QPixmap::fromImage(QImage(tempImage.data, tempImage.cols, tempImage.rows, tempImage.step, QImage::Format_Grayscale8));
    int w = ui->label->width();
    int h = ui->label->height();
    ui->label->setPixmap(p.scaled(w,h,Qt::KeepAspectRatio));
}
