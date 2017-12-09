// bgFaceDetect.cpp : �������̨Ӧ�ó������ڵ㡣
//
// ����������չʾʶ��ͼƬ�е�����
// --img=D:\hezhao.jpg
// --face_data=E:\Project\OpenSource\OpenCV\opencv\data\haarcascades\haarcascade_frontalface_default.xml
// --eyes_data=E:\Project\OpenSource\OpenCV\opencv\data\haarcascades\haarcascade_eye_tree_eyeglasses.xml

#include "stdafx.h"

#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>
#include <vector>
#include <atlconv.h>

int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf("usage : %s <img_url> <factor_url>\n", argv[0]);
		return 0;
	}

	USES_CONVERSION;

	TCHAR img_url[4096] = {0};
	_tcscpy_s(img_url, 4096, argv[1]);

	TCHAR factor_url[4096] = {0};
	_tcscpy_s(factor_url, 4096, argv[2]);

	// ����ͼƬ
	cv::Mat img_frame = cv::imread(T2A(img_url));

	// ��������ֵ
	// ����������
	cv::CascadeClassifier face_cascade;
	bool result = face_cascade.load(T2A(factor_url));
	if (!result)
	{
		printf("Load face factor failed...\n");
		return -1;
	}

	// ��������

	// ��ʼ����ʱ��
	int64 tick_start = cv::getTickCount();

	// �Ƚ������û�
	cv::Mat img_frame_gray;
	cv::cvtColor(img_frame, img_frame_gray, cv::COLOR_BGR2GRAY);

	std::vector<cv::Rect> faces;
	face_cascade.detectMultiScale(img_frame_gray, faces, 1.1, 3, CV_HAAR_DO_ROUGH_SEARCH, cv::Size(10, 10), cv::Size(800, 800));

	std::vector<cv::Rect>::iterator iter;
	for (iter = faces.begin(); iter != faces.end(); ++iter)
	{
		// ��ͼƬ�б�ʶ����
		cv::rectangle(img_frame, *iter, cv::Scalar(255, 0, 0), 2, 8, 0);

		// �����������ڴ˷�Χ�ڲ�������
	}
	// �����ʱ
	int64 tick_end = cv::getTickCount();

	printf("time : %d ms\n", tick_end - tick_start);

	// ���ʶ�𵽵�����ͼƬ
	cv::namedWindow("bgFaceDetect");
	cv::imshow("bgFaceDetect", img_frame);
	cv::waitKey();

	return 0;
}

