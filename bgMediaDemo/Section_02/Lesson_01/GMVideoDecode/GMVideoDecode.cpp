// GMVideoDecode.cpp : �������̨Ӧ�ó������ڵ㡣
//
// ��������FLV��Ƶ�ļ�����ΪYUV�ļ���PCM�ļ�

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
	if (argc < 2)
	{
		printf("GMVideoDecode.exe <video_in_path>\n");
		return 0;
	}

	// ��ɳ�ʼ��ע��
	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	TCHAR video_in_path[4096] = {0};
	_tcscpy_s(video_in_path, 4096, argv[1]);

	USES_CONVERSION;
	AVFormatContext *input_format_context = NULL;
	int errCode = avformat_open_input(&input_format_context, T2A(video_in_path), NULL, NULL);
	if (errCode != 0)
	{
		printf("Open video file failed...\n");
		return errCode;
	}

	errCode = avformat_find_stream_info(input_format_context, NULL);
	if (errCode < 0)
	{
		printf("Find video's stream information failed...\n");
		avformat_close_input(&input_format_context);
		return errCode;
	}

	int input_video_stream_index = -1;
	int input_audio_stream_index = -1;
	AVStream *input_video_stream = NULL;
	AVStream *input_audio_stream = NULL;
	AVCodecContext *input_video_codec_context = NULL;
	AVCodecContext *input_audio_codec_context = NULL;

	for (int index = 0; index < input_format_context->nb_streams; ++index)
	{
		if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			input_audio_stream_index	= index;
			input_audio_stream			= input_format_context->streams[index];
			input_audio_codec_context	= input_audio_stream->codec;
		}
		else if (input_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			input_video_stream_index	= index;
			input_video_stream			= input_format_context->streams[index];
			input_video_codec_context	= input_video_stream->codec;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// ׼���������Ľ�����
	//
	//////////////////////////////////////////////////////////////////////////
	AVCodec *input_video_decoder = NULL;
	AVCodec *input_audio_decoder = NULL;
	if (input_video_codec_context >= 0)
	{
		input_video_decoder = avcodec_find_decoder(input_video_codec_context->codec_id);
		if (input_video_decoder == NULL)
		{
			printf("Not found video decoder...\n");
			return -2;
		}

		errCode = avcodec_open2(input_video_codec_context, input_video_decoder, NULL);
		if (errCode != 0)
		{
			printf("Open video decoder failed. %d\n", errCode);
			return errCode;
		}
	}
	
	if (input_audio_stream_index >= 0)
	{
		input_audio_decoder = avcodec_find_decoder(input_audio_codec_context->codec_id);
		if (input_audio_decoder == NULL)
		{
			printf("Not found audio decoder...\n");
			return -2;
		}

		errCode = avcodec_open2(input_audio_codec_context, input_audio_decoder, NULL);
		if (errCode != 0)
		{
			printf("Open audio decoder failed. %d\n", errCode);
			return errCode;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// ��Ƶͼ��ת����ǰ�ڲ�����׼��ת��ΪYUV�ļ�
	//
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	// ��Ƶת����ǰ�ڲ���������Ƶת��ΪPCM�ļ�
	//
	//////////////////////////////////////////////////////////////////////////

	return 0;
}

