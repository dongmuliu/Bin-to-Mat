// binshow.cpp : 定义控制台应用程序的入口点。
//

#include <stdio.h>
#include <tchar.h>
#include <vector>
#include <list>
#include <conio.h>
#include <fstream>
#include <opencv2\opencv.hpp>

using namespace cv;
using namespace std;
#define   Img_width   (320)
#define   Img_height  (240)
#define   Max_distance 8000
#define   Min_distacne 0
#define IMG_B(img,y,x) img.at<Vec3b>(y,x)[0]
#define IMG_G(img,y,x) img.at<Vec3b>(y,x)[1]
#define IMG_R(img,y,x) img.at<Vec3b>(y,x)[2]
ushort  tmp;
Mat		img_color;

void on_mouse(int event, int x, int y, int flags, void* userdata)
{
	Mat hh;
	char file[64];
	hh = *(Mat*)userdata;
	Point p(x, y);
	if (event == EVENT_LBUTTONDOWN)
	{
		//sprintf(file, "(%d)", (int)(hh.at<uchar>(p)));
		//putText(src, file, p, FONT_HERSHEY_PLAIN, 1, Scalar(255), 1, 8);
		printf("b=%0.3f\n", hh.at<ushort>(p)/30000.0*12.5);
		printf("n=%d\n", hh.at<ushort>(p));
		// circle(hh, p, 2, Scalar(255), 3);
	}
	else if (event == CV_EVENT_RBUTTONDOWN)
	{
		circle(hh, p, 2, Scalar(255), 3);
	}


}


Mat  getDataFromfile(const char * filename)
{

	ifstream myi(filename, ios::binary);
	unsigned short * imagebuf = new unsigned short[Img_width*Img_height];
	Mat outmat(Img_height, Img_width, CV_16UC1);
	if (!myi.is_open())
	{
		return outmat;
	}
	myi.read((char*)imagebuf, Img_width*Img_height*sizeof(unsigned short));
	myi.close();


	int nline = 0;


	for (int i = 0; i < Img_height; i++)
	{
		for (int j = 0; j < Img_width; j++)
		{
			if (imagebuf[j + i * Img_width] < 30000)
				outmat.at<ushort>(i, j) = imagebuf[j + i * Img_width];
			else
				outmat.at<ushort>(i, j) = 30000;
		  // outmat.at<uchar>(i, j) = 255 - (imagebuf[j + i * Img_width]) * 255 /30000;

		}
	}

	delete[] imagebuf;
	return outmat;
}

int _tmain(int argc, _TCHAR* argv[])
{


	bool bsave = 0;
	char imgfilename85[_MAX_PATH];
	char outfilename[_MAX_PATH];
	_TCHAR inipath[_MAX_PATH];
	_TCHAR  depthcfgpath[_MAX_PATH];

	_TCHAR drive[_MAX_DRIVE];
	_TCHAR dir[_MAX_DIR];
	//	char	szdepthcfgpath[_MAX_PATH];
	_tsplitpath(argv[0], drive, dir, NULL, NULL);
	sprintf(imgfilename85, "H:\\Laboratory project\\w_test task\\103743\\CAM1_2.bin");
	
	Mat outmat85 = getDataFromfile(imgfilename85);

	if (outmat85.empty())
	{
		cout << "err when open file!" << endl;
		return -1;
	}
	
	
	
	Mat zip;
	outmat85.convertTo(zip, CV_8U, 25.0 / 3000, 0);
	double mindistance = Min_distacne*25.0 / 3000;
	double maxdistance = Max_distance*25.0 / 3000;
	img_color.create(Img_height, Img_width, CV_8UC3);//构造RGB图像
	//////利用Opecv进行伪彩色映射//////////////////
	//applyColorMap(zip, zip, COLORMAP_HSV);
	//applyColorMap(zip, zip, COLORMAP_RAINBOW);
	////////////////////////////////////
	for (int y = 0; y < Img_height; y++)
	{
		for (int x = 0; x < Img_width; x++)
		{
			tmp = zip.at<uchar>(y, x);
			if (tmp < mindistance)
			{
				tmp = mindistance;
			}
			else if (tmp >maxdistance)
			{
				tmp = maxdistance;
			}
			double intervaldistance = (maxdistance - mindistance) / 5;
			if (tmp < intervaldistance)
			{
				IMG_B(img_color, y, x) = 0;
				IMG_G(img_color, y, x) = tmp * 255/intervaldistance;
				IMG_R(img_color, y, x) = 0;
			}
			else if (tmp < 2 * intervaldistance)
			{
				tmp -= intervaldistance;
				IMG_B(img_color, y, x) = 255 - tmp * 255 / intervaldistance;
				IMG_G(img_color, y, x) = 255;
				IMG_R(img_color, y, x) = 0;
			}
			else if (tmp < 3 * intervaldistance)
			{
				tmp -= 2 * intervaldistance;
				IMG_B(img_color, y, x) = 0;
				IMG_G(img_color, y, x) = 255;
				IMG_R(img_color, y, x) = tmp * 255 / intervaldistance;
			}
			else if (tmp <= 4*intervaldistance)
			{
				tmp -= 3*intervaldistance;
				IMG_B(img_color, y, x) = 0;
				IMG_G(img_color, y, x) = 255 - uchar(128.0*tmp /intervaldistance + 0.5);
				IMG_R(img_color, y, x) = 255;
			}
			else
			{
				tmp -= 4 * intervaldistance;
				IMG_B(img_color, y, x) = 0;
				IMG_G(img_color, y, x) = 127 - uchar(127.0*tmp/intervaldistance+ 0.5);
				IMG_R(img_color, y, x) = 255;
			}
		}
	}


	namedWindow("win1");
	imshow("win1", img_color);
	setMouseCallback("win1", on_mouse, &outmat85);
	//imwrite("CAM1_0.bmp", outmat85);
	waitKey(0);
	return 0;
}



