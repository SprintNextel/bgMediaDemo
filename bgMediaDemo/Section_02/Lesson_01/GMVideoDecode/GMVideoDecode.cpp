// GMVideoDecode.cpp : �������̨Ӧ�ó������ڵ㡣
//
// ��������MP4��Ƶ�ļ�����ΪYUV�ļ���PCM�ļ�

#include "stdafx.h"

#include <atlconv.h>

#define __STDC_CONSTANT_MACROS

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libavutil/rational.h"
#include "libavutil/pixdesc.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
#ifdef __cplusplus
};
#endif

#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf("GMVideoDecode.exe <video_in_path> <video_out_path> \n");
		return 0;
	}

	// ��ɳ�ʼ��ע��
	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	TCHAR video_in_path[4096] = {0};
	_tcscpy_s(video_in_path, 4096, argv[1]);

	TCHAR video_out_path[4096] = {0};
	_tcscpy_s(video_out_path, 4096, argv[2]);

	USES_CONVERSION;

	return 0;
}

