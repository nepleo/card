#include <iostream>
#include <opencv2/opencv.hpp>
#include "card.h"
using namespace std;
using namespace cv;


int main()
{
    // srcImage 为输入图像， dstImage 为输出图像
    Mat srcImage, dstImage;
    //GARY 为灰度图像
    Mat temp1, temp2, GARY;
    srcImage = imread("./srcimg/1.jpg");  
    //灰度变换
    BGR2GARY(srcImage, GARY, 1);
    imshow("GARY", GARY);
    // tmpHist1 为灰度的直方图 
    Mat tmpHist1 = drawHist(GARY);
    imshow("灰度图像直方图", tmpHist1);
    //temp1 为灰度图像均衡化处理之后的图像
    equalHist(GARY,temp1);
    imshow("temp1", temp1);
    Mat tmpHist2 = drawHist(temp1);
    imshow("均衡化之后灰度图像直方图", tmpHist2);
    enhanContrast(temp1, temp1);
    imshow("图像增强对比度", temp1);
    medianFilter(temp1, temp1);
    imshow("图像中值滤波", temp1);
    binarization(temp1, temp1, 35);
    imshow("二值化", temp1);
    // temp2  为边缘处理之后，方便寻找位置
    Sobel(temp1, temp2);
    imshow("Sobel", temp2);
    // r为坐标，可根据roughPosX 的返回来判断
    int r[2] = { 0 };
    roughPosX(temp2, 25, r);
    line(srcImage, Point(0, r[0] - 2), Point(640, r[0] - 2), Scalar(0, 255, 255), 1, CV_AA);
    line(srcImage, Point(0, r[1] + 2), Point(640, r[1] + 2), Scalar(0, 255, 255), 1, CV_AA);
    Rect rect(0, r[0] - 2, temp1.cols, r[1] - r[0] + 4);

    //cut 为切割后的图像
    Mat cut = temp1(rect);
    Mat cutclone = cut.clone();
    imshow("cut", cut);
    Erode(cut, cut, 3, 3);
    imshow("cut-Erode", cut);
    //先膨胀，后腐蚀
    Dilate(cut, cut, 13, 17);
    Erode(cut, cut, 5, 5);
    imshow("cut-Dilate", cut);
    int axis[4];
    roughPosY(cut, 9, axis);
    line(srcImage, Point(axis[0], r[0] - 2), Point(axis[0], r[1] + 2), Scalar(0, 0, 255), 1, CV_AA);
    line(srcImage, Point(axis[1], r[0] - 2), Point(axis[1], r[1] + 2), Scalar(0, 0, 255), 1, CV_AA);
    line(srcImage, Point(axis[2], r[0] - 2), Point(axis[2], r[1] + 2), Scalar(0, 0, 255), 1, CV_AA);
    line(srcImage, Point(axis[3], r[0] - 2), Point(axis[3], r[1] + 2), Scalar(0, 0, 255), 1, CV_AA);
    Rect rect1(axis[0], 0, axis[1] - axis[0], cut.rows);
    //cut1 为卡号第一段
    Mat cut1 = cutclone(rect1);
    Rect rect2(axis[2], 0, axis[3] - axis[2], cut.rows);
    //cut2 为卡号第二段
    Mat cut2 = cutclone(rect2);
    imshow("cut1", cut1);
    imshow("cut2", cut2);
    imshow("srcImage", srcImage);

    vector<int> Char;  //存储初步分割出来的字符的宽度
    vector<int> start; //存储初步分割出来的字符的起始位置

    int num1 = 0;
    splitChar(cut1, num1, Char, start,7, 5.0, 6);
    cout << num1 << endl;

    int num2 = 0;
    vector<int> Char2;  //存储初步分割出来的字符的宽度
    vector<int> start2; //存储初步分割出来的字符的起始位置
    splitChar(cut2, num2, Char2, start2, 3, 5.0, 13);
    cout << num2 << endl;
    
    //卡号第一部分切割后图像
    Mat tmp[15];
    for (int i = 0; i < num1; i++) {
        Rect rect(start[i], 0, Char[i], cut1.rows);
        tmp[i] = cut1(rect);
        string str = "cut1-" + to_string(i);
        imshow(str, tmp[i]);
    }
    //卡号第二部分切割后图像
    Mat tmp2[15];
    for (int i = 0; i < num2; i++) {
        Rect rect(start2[i], 0, Char2[i], cut2.rows);
        tmp2[i] = cut2(rect);
        string str = "cut2-" + to_string(i);
        imshow(str, tmp2[i]);
    }

    //读取模板图像
    vector<Mat> train;
    for (int i = 0; i < 10; i++) {
        string str = "./train/Sam" + to_string(i) + ".jpg";
        train.push_back(imread(str));
    }

    for (int i = 0; i < num1; i++) {
        int r = 0;
        r = matchChar(tmp[i], train);
        cout << r;
    }
    cout << " ";

    for (int i = 0; i < num2; i++) {
        int r = 0;
        r = matchChar(tmp2[i], train);
        cout << r;
    }

    waitKey(0);
    return 0;
}



