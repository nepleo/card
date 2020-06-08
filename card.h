#include <iostream>
#include <opencv2/opencv.hpp>
#include <map>
#include <algorithm>
#include <vector>
using namespace cv;
using namespace std;

void BGR2GARY(Mat srcImage, Mat& dstImage, int flag);
void drawGaryHist(Mat img);
Mat drawHist(Mat img);
void equalHist(Mat src, Mat& dst);
void enhanContrast(Mat src, Mat& dst);
void medianFilter(Mat src, Mat& dst);
void binarization(Mat src, Mat& dst, int threshold);
void Roberts(Mat src, Mat& dst);
void Prewitt(Mat src, Mat& dst);
void Sobel(Mat src, Mat& dst);
void roughPosX(Mat src, int num, int r[]);
void Dilate(Mat src, Mat& dst, int em, int en);
void Erode(Mat src, Mat& dst, int em, int en);
void roughPosY(Mat src, int num, int* r);
void splitChar(Mat src, int& num, vector<int>& Char, vector<int>& start, int t1, float t2, int prenum);
int matchChar(Mat src, vector<Mat> train);