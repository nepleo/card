#include "card.h"


//排序
typedef pair<int, float> P;
bool cmp( P a, P b) {
	return a.second > b.second;
}

/*
* Function ：BGR2GARY(Mat srcImage, Mat& dstImage, int flag)
* Description : 将彩色图像转换为灰度图像
* Input : srcImage, dstImage， flag
* Output : none
* Return : void
* Other : flag = 0 时，平均值法求灰度图像 
		  flag = 1 时，最大值法求灰度图像
		  flag = 2 时，加权法求灰度图像
*/
void BGR2GARY(Mat srcImage, Mat& dstImage, int flag) {
	dstImage = Mat(srcImage.rows, srcImage.cols, CV_8UC1);
	int rows = srcImage.rows;
	int cols = srcImage.cols;
	for (int i = 0; i < rows; i++) {
		for (int j = 0; j < cols; j++) {
			uchar b = srcImage.at<Vec3b>(i, j)[0];
			uchar g = srcImage.at<Vec3b>(i, j)[1];
			uchar r = srcImage.at<Vec3b>(i, j)[2];
			if (flag == 0) {
				// 平均值法
				dstImage.at<uchar>(i, j) = (b + g + r) / 3;
			}
			else if (flag == 1) {
				//最大值
				uchar temp = max(r, g);
				dstImage.at<uchar>(i, j) = max(temp, b);
			}
			else if (flag == 2) {
				//加权
				dstImage.at<uchar>(i, j) = (uchar)(0.3 * r + 0.6 * g + 0.1 * b);
			}
		}
	}

}

/*
* Function ：void drawGaryHist(Mat img)
* Description ：画出灰度直方图
* Input : img
* Others: 计算直方图
*/
void drawGaryHist(Mat img) {
	int bins = 256;
	int hist_size[] = { bins };
	float range[] = { 0, 256 };
	const float* ranges[] = { range };
	MatND hist;
	int channels[] = { 0 };
	//创建直方图
	calcHist(&img, 1, channels, Mat(), hist, 1, hist_size, ranges, true, false);
	double maxVal;
	minMaxLoc(hist, 0, &maxVal, 0, 0);
	int scale = 2;
	int histHeight = 300; //直方图高度
	Mat histImg = Mat::zeros(histHeight, bins * scale, CV_8UC3);
	for (int i = 0; i < bins; i++) {
		float binVal = hist.at<float>(i);
		int inten = cvRound(binVal * histHeight / maxVal);
		rectangle(histImg, Point(i * scale, histHeight), Point((i + 1) * scale - 1, histHeight - inten), CV_RGB(0, 255, 255));
	}
	imshow("hist", histImg);
}

/*
* Function ：Mat drawHist(Mat img)
* Description ：画出直方图
* Input : img
* Return ：Mat 图像
* Others: 计算直方图
*/
Mat drawHist(Mat img) {
	const int histSize = 256;
	int hist[histSize];
	fill(hist, hist + histSize, 0);   //初始化为0
	int row = img.rows;
	int col = img.cols;
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			hist[img.at<uchar>(i, j)]++;
		}
	}
	//找出直方图中的最大值，方便归一化
	int histMax = 0;
	for (int i = 0; i < 256; i++) {
		if (hist[i] > histMax) {
			histMax = hist[i];
		}
	}
	//创建空白图像
	int histHeight = 300;
	Mat histImg = Mat::zeros(histHeight, histSize * 2, CV_8UC3);
	//画出灰度直方图
	for (int i = 0; i < 256; i++) {
		float Val = (float)hist[i];
		int inten = cvRound(Val * histHeight / histMax);
		Point p0 = Point(i * 2, histHeight);
		Point p1 = Point((i + 1) * 2 - 1, histHeight - inten);
		rectangle(histImg, p0, p1, CV_RGB(0, 255, 255));
	}
	return histImg;
}

/*
* Function ：equalHist(Mat src, Mat& dst)
* Description ：直方图均衡化
*/
void equalHist(Mat src, Mat& dst) {
	const int histSize = 256;
	int row = src.rows;
	int col = src.cols;
	dst = Mat::zeros(row, col, CV_8UC1);
	int hist[histSize];
	fill(hist, hist + histSize, 0);
	//扫描每一个像素点，统计每一个灰度级出现的次数
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			hist[src.at<uchar>(i, j)]++;
		}
	}
	//累积函数
	int cal[histSize];
	cal[0] = hist[0];
	for (int i = 1; i < histSize; i++) {
		cal[i] = cal[i - 1] + hist[i];
	}
	// 根据映射函数，建立 look up table
	uchar lut[histSize];
	for (int i = 0; i < histSize; i++) {
		int val = cvRound(cal[i] * (255.f / (row * col)));
		lut[i] = (uchar)(val);
	}
	//根据look up table ,改变函数像素值
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			dst.at<uchar>(i, j) = lut[src.at<uchar>(i, j)];
		}
	}
}

/*
* Function ：enhanContrast(Mat src, Mat& dst)
* Description : 线性灰度变换
* Other： 增强对比度
*/
void enhanContrast(Mat src, Mat& dst) {
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	int r1 = 0, r2 = 0;
	int s1 = 0, s2 = 255;
	int hist[256];
	fill(hist, hist + 256, 0);
	//统计灰度级
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			hist[src.at<uchar>(i, j)]++;
		}
	}
	//计算控制点坐标
	long long area = row * col;
	int sum = 0;
	for (int i = 0; i < 256; i++) {
		sum += hist[i];
		if (sum > area * 0.1) {
			r1 = i;
			break;
		}
	}
	sum = 0;
	for (int i = 255; i > 0; i--) {
		sum += hist[i];
		if (sum > area * 0.1) {
			r2 = i;
			break;
		}
	}
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			uchar pix = src.at<uchar>(i, j);
			if (pix >= 0 && pix < r1) {
				dst.at<uchar>(i, j) = (uchar)(pix * s1 / r1);
			}
			else if (pix >= r1 && pix <= r2) {
				dst.at<uchar>(i, j) = (uchar)((s2 - s1) / (r2 - r1) * (pix - r1) + s1);
			}
			else if (pix > r2&& pix <= 255) {
				dst.at<uchar>(i, j) = (uchar)((255 - s2) / (255 - r2) * (pix - r2) + s2);
			}
			
		}
	}
}

/*
* Function : void medianFilter(Mat src, Mat& dst)
* Description : 中值滤波器， 3 * 3 模板
*/
void medianFilter(Mat src, Mat& dst) {
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	for (int i = 1; i < row - 1; i++) {
		for (int j = 1; j < col - 1; j++) {
			int a0 = src.at<uchar>(i - 1, j - 1);
			int a1 = src.at<uchar>(i - 1, j);
			int a2 = src.at<uchar>(i - 1, j + 1);
			int a3 = src.at<uchar>(i, j - 1);
			int a4 = src.at<uchar>(i, j + 1);
			int a5 = src.at<uchar>(i + 1, j - 1);
			int a6 = src.at<uchar>(i + 1, j);
			int a7 = src.at<uchar>(i + 1, j + 1);
			int a8 = src.at<uchar>(i, j);
			float ave = (a0 + a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8) / 9.0;
			//以a8 为中心的 “十字型” 模板 取中值
			int mid_ten[5] = { a1, a3, a4, a6, a8 };
			sort(mid_ten, mid_ten + 5);
			int midOne = mid_ten[2];
			int mid_x[5] = { a0, a2, a5, a7, a8 };
			sort(mid_x, mid_x + 5);
			int midTwo = mid_x[2];
			int deltaOne = midOne - a8;
			int deltaTwo = midTwo - a8;
			int deltaThr = a8 - (uchar)ave;
			if (deltaOne == deltaTwo) {
				dst.at<uchar>(i, j) = midOne;
			}
			else if (deltaThr >= 0 && deltaOne != deltaTwo) {
				dst.at<uchar>(i, j) = max(midOne, midTwo);
			}
			else if(deltaThr < 0 && deltaOne != deltaTwo){
				dst.at<uchar>(i, j) = min(midOne, midTwo);
			}
		}
	}
}

/*
* Function : binarization(Mat src, Mat& dst)
* Description : 将图像二值化
*/
void binarization(Mat src, Mat& dst , int threshold) {
	if (threshold < 0 || threshold > 255) {
		dst = src.clone();
		return;
	}
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < col; j++) {
			int pix = src.at<uchar>(i, j);
			if (pix > threshold) {
				dst.at<uchar>(i, j) = 0;
			}
			else {
				dst.at<uchar>(i, j) = 255;
			}
		}
	}
}

/*
* Description ： Roberts 算子边缘检测
* Function ：Roberts(Mat src, Mat& dst)
*/
void Roberts(Mat src, Mat& dst) {
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	for (int i = 1; i < row - 1; i++) {
		for (int j = 1; j < col -1 ; j++) {
			uchar a1 = src.at<uchar>(i, j);
			uchar a2 = src.at<uchar>(i + 1, j + 1);
			uchar a3 = src.at<uchar>(i, j + 1);
			uchar a4 = src.at<uchar>(i + 1, j);
			dst.at<uchar>(i, j) = abs(a1 - a2) + abs(a3 - a4);
		}
	}
}

/*
* Function : Prewitt(Mat src, Mat& dst)
* Description ： Prewitt 算子边缘检测
*/
void Prewitt(Mat src, Mat& dst) {
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	for (int i = 1; i < row - 1; i++) {
		for (int j = 1; j < col - 1; j++) {
			//Gx
			int xf1 = src.at<uchar>(i + 1, j - 1);
			int xf2 = src.at<uchar>(i + 1, j);
			int xf3 = src.at<uchar>(i + 1, j + 1);
			int xf4 = src.at<uchar>(i - 1, j - 1);
			int xf5 = src.at<uchar>(i - 1, j);
			int xf6 = src.at<uchar>(i - 1, j + 1);
			//Gy
			int yf1 = src.at<uchar>(i - 1, j + 1);
			int yf2 = src.at<uchar>(i, j + 1);
			int yf3 = src.at<uchar>(i + 1, j + 1);
			int yf4 = src.at<uchar>(i - 1, j - 1);
			int yf5 = src.at<uchar>(i, j - 1);
			int yf6 = src.at<uchar>(i + 1, j + 1);
			int Gx = xf1 + xf2 + xf3 - xf4 - xf5 - xf6;
			int Gy = yf1 + yf2 + yf3 - yf4 - yf5 - yf6;
			dst.at<uchar>(i, j) = (uchar)abs(Gx) + (uchar)abs(Gy);
		}
	}
}

/*
* Function : Sobel(Mat src, Mat& dst)
* Description ： Sobel 算子边缘检测
*/
void Sobel(Mat src, Mat& dst) {
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	for (int i = 1; i < row - 1; i++) {
		for (int j = 1; j < col - 1; j++) {
			//Gx
			int xf1 = src.at<uchar>(i + 1, j - 1);
			int xf2 = src.at<uchar>(i + 1, j);
			int xf3 = src.at<uchar>(i + 1, j + 1);
			int xf4 = src.at<uchar>(i - 1, j - 1);
			int xf5 = src.at<uchar>(i - 1, j);
			int xf6 = src.at<uchar>(i - 1, j + 1);
			//Gy
			int yf1 = src.at<uchar>(i - 1, j + 1);
			int yf2 = src.at<uchar>(i, j + 1);
			int yf3 = src.at<uchar>(i + 1, j + 1);
			int yf4 = src.at<uchar>(i - 1, j - 1);
			int yf5 = src.at<uchar>(i, j - 1);
			int yf6 = src.at<uchar>(i + 1, j + 1);
			int Gx = xf1 + 2 * xf2 + xf3 - xf4 - 2 * xf5 - xf6;
			int Gy = yf1 + 2 * yf2 + yf3 - yf4 - 2 * yf5 - yf6;
			dst.at<uchar>(i, j) = (uchar)abs(Gx) + (uchar)abs(Gy);
		}
	}
}


/*
* Function: roughPos(Mat src, int num, int r[])
* Description ： 设定num 为阈值，找到银行卡号的位置
* Other ： 从下向上遍历，将卡号的上下两行的行数存在 r 数组中
*/
void roughPosX(Mat src, int num, int r[]) {
	int row = src.rows;
	int col = src.cols;
	int count[10000];
	vector<int> k;
	fill(count, count + 10000, 0);
	for (int i = row - 1; i > 1; i--) {
		for (int j = 1; j < col - 1; j++) {
			int pix1 = src.at<uchar>(i, j);
			int pix2 = src.at<uchar>(i, j + 1);
			if (abs(pix1 - pix2) / 255) {
				count[i]++;
			}
		}
	}
	for (int i = 0; i < 10000; i++) {
		if (count[i] > num) {
			k.push_back(i);
			//cout << i << endl;
		}
	}
	r[0] = k[0];
	r[1] = k[k.size() - 1];
}

/*
* Function ：Dilate(Mat src, Mat& dst, int em, int en)
* Description ：对二值图像膨胀操作
* Input : em : 结构元素的行数， en : 结构元素的列数， 结构元素为矩阵
*/
void Dilate(Mat src, Mat& dst, int em, int en) {
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	//（cem， cen) 矩阵元素的中心点 
	int cem = (em - 1) / 2;
	int cen = (en - 1) / 2;
	//跳出两层循环标志位
	int breakflag = 0;

	// 为防止越界
	for (int i = cem; i < row - cem - 1; i++) {
		for (int j = cen; j < col - cen - 1; j++) {
			// i -cem， j - cen 为结构第一个元素
			for (int m = 0; m < em; m++) {
				for (int n = 0; n < en; n++) {
					if (src.at<uchar>(i - cem + m, j - cen + n) == 255) {
						dst.at<uchar>(i, j) = 255;
						breakflag = 1;
						break;
					}
				}
				if (breakflag == 1) {
					break;
				}
			}
		}
	}
}

/*
* Function ：Erode(Mat src, Mat& dst, int em, int en)
* Description ：对二值图像腐蚀操作
* Input : em : 结构元素的行数， en : 结构元素的列数， 结构元素为矩阵
*/
void Erode(Mat src, Mat& dst, int em, int en) {
	int row = src.rows;
	int col = src.cols;
	dst = src.clone();
	//（cem， cen) 矩阵元素的中心点 
	int cem = (em - 1) / 2;
	int cen = (en - 1) / 2;
	//跳出两层循环标志位
	int breakflag = 0;

	// 为防止越界
	for (int i = cem; i < row - cem - 1; i++) {
		for (int j = cen; j < col - cen - 1; j++) {
			breakflag = 0;
			// i -cem， j - cen 为结构第一个元素
			for (int m = 0; m < em; m++) {
				for (int n = 0; n < en; n++) {
					if (src.at<uchar>(i - cem + m, j - cen + n) == 0) {
						dst.at<uchar>(i, j) = 0;
						breakflag = 1;
						break;
					}
				}
				if (breakflag == 1) {
					break;
				}
			}
		}
	}
}

/*
* Function: roughPosY(Mat src, int num, int r[])
* Description ： 找到卡号的位置
* Other ： 从左到右，从上到下遍历，统计白色像素点的值
*/
void roughPosY(Mat src, int num, int* r) {
	int row = src.rows;
	int col = src.cols;
	int count[10000];
	fill(count, count + 10000, 0);
	// k 存储col 方向的值
	vector<int> k;
	for (int j = 0; j < col; j++) {
		for (int i = 0; i < row; i++) {
			if (src.at<uchar>(i, j) == 255) {
				count[j]++;
			}
		}
	}
	for (int i = 0; i < 10000; i++) {
		if (count[i] > num) {
			k.push_back(i);
		}
	}
	for (int i = 1; i < k.size() - 1; i++) {
		if (k[i] - k[i - 1] > 15 && k[i] - k[i - 1] < 40) {
			r[1] = k[i - 1];  //第一个方框右边的值
			r[2] = k[i];      //第二个方框左边的值
			//break;
		}
		if (k[i] - k[i - 1] > 40) {
			r[3] = k[i - 1];
		}
	}
	r[0] = k[0];
}

/*
* Function : splitChar(...)
* Description :分割字符
* Other : t1 为每一列像素点个数阈值， t2 为高宽比， prenum 为预知的字符个数
*/
void splitChar(Mat src, int &num, vector<int>& Char, vector<int>&start, int t1, float t2, int prenum) {
	int row = src.rows;
	int col = src.cols;
	int count[1000];
	num = 0;  //记录分割出来的字符数目
	//int t1 = 4; //字符区域的阈值
	//float t2 = 5.0; //字符高宽比阈值
 	fill(count, count + 1000, 0);
	int nStart = -1; //待考察字符区域开始的位置，没有则为-1
	for (int j = 0; j < col; j++) {
		for (int i = 0; i < row; i++) {
			if (src.at<uchar>(i, j) == 255) {
				count[j]++;
			}
		}
	}
	for (int i = 0; i < 1000; i++) {
		if (count[i] >= t1) {
			if (nStart == -1) {
				nStart = i;
			}
			continue;
		}
		else {
			if (nStart != -1) {
				float rate = row / (float)(i - nStart);
				if (rate <= t2) {
					Char.push_back(i - nStart);  //存储初步分割出来的字符的宽度
					start.push_back(nStart);     //存储初步分割出来的字符的起始位置
					num++;
					nStart = -1;
				}
				else {
					nStart = -1;
				}
			}
		}
	}
	//如果识别到的个数小于预期个数
	if (num < prenum) {
		int w = Char[(num - 1) / 2];
		for (int i = 0; i < num; i++) {
			if (Char[i] > 2 * w) {
				Char.insert(Char.begin() + i + 1, Char[i] - (int)(1 + 12 / 45.0) * w);
				Char[i] = w;
				start.insert(start.begin() + i + 1, start[i] + w + (int)(12.0 / 45.0) * w);
				num++;
			}
		}
	}
}

/*
* Function :int matchChar(Mat src, vector<Mat> train)
* Description: 此函数为字符匹配函数
* Input : src 进行匹配的函数， train 为匹配函数的字符集
*/
int matchChar(Mat src, vector<Mat> train) {
	//字符集的高度
	int trainRow = train[0].rows;
	int trainCol = train[0].cols;
	//对字符集归一化的高度
	int row = src.rows;
	int col = src.cols; //宽
	float c1 = (float)col / (float)trainCol; //宽
	float c2 = (float)row / (float)trainRow; //高

	//对src 归一化
	Mat nomSrc = Mat::zeros(trainRow, trainCol, CV_8UC1);
	for (int i = 0; i < trainRow - 1; i++) {
		for (int j = 0; j < trainCol - 1; j++) {
			nomSrc.at<uchar>(i, j) = src.at<uchar>(cvFloor(c2 * i + 0.5), cvFloor(c1 * j + 0.5));
		}
	}

	//模板图像预处理，变为一通道,和二值图像
	for (int num = 0; num < train.size(); num++) {
		cvtColor(train[num], train[num], CV_BGR2GRAY);
		for (int i = 0; i < train[num].rows; i++) {
			for (int j = 0; j < train[num].cols; j++) {
				if (train[num].at<uchar>(i, j) > 200) {
					train[num].at<uchar>(i, j) = 255;
				}
				else {
					train[num].at<uchar>(i, j) = 0;
				}
			}
		}
	}
	

	//模板匹配,根据公式返回一个比率
	vector<pair<int, float>>s;
	for (int num = 0; num < train.size(); num++) {
		int molecule = 0; //分子
		int deno1 = 0;
		int deno2 = 0; // 分母
		for (int i = 1; i < trainRow; i++) {
			for (int j = 1; j < trainCol; j++) {
				molecule += nomSrc.at<uchar>(i, j) * train[num].at<uchar>(i, j);
				deno1 += pow(nomSrc.at<uchar>(i, j), 2);
				deno2 += pow(train[num].at<uchar>(i, j), 2);
			}
		}
		float deno = (float)sqrt(deno1) * (float)sqrt(deno2);
		float rate = molecule / deno;
		s.push_back(pair<int, float>(num, rate));
	}
	sort(s.begin(), s.end(), cmp);
	return s[0].first;
}
