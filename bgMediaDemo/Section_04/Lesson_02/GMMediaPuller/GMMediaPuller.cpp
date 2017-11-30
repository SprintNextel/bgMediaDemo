// GMMediaPuller.cpp : �������̨Ӧ�ó������ڵ㡣
//
// ����������ȡRTSP/RTMP/HLS����������ΪMP4��flv�ļ�
// 
// ��������ַ��
// ������ӣ�rtmp://live.hkstv.hk.lxdns.com/live/hks
// �����ã�VOD����rtsp://184.72.239.149/vod/mp4://BigBuckBunny_175k.mov
// �������̨��rtsp://rtsp-v3-spbtv.msk.spbtv.com/spbtv_v3_1/214_110.sdp
// ������ӣ�http://live.hkstv.hk.lxdns.com/live/hks/playlist.m3u8

#include "stdafx.h"

#include <atlconv.h>

#define __STDC_CONSTANT_MACROS

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
#include "libavutil/avutil.h"
#include "libavutil/rational.h"
#include "libavutil/pixdesc.h"
#include "libavutil/samplefmt.h"
#include "libswresample/swresample.h"
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

#include <iostream>


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf("%s <stream_url> <local_url>\n", argv[0]);
		return 0;
	}

	int errCode = 0;

	TCHAR stream_url[4096] = {0};
	_tcscpy_s(stream_url, 4096, argv[1]);

	TCHAR local_url[4096] = {0};
	_tcscpy_s(local_url, 4096, argv[2]);

	USES_CONVERSION;

	av_register_all();
	avformat_network_init();

	//////////////////////////////////////////////////////////////////////////
	//
	// ����
	//
	//////////////////////////////////////////////////////////////////////////

	AVDictionary *options = NULL;
	AVFormatContext *stream_format_context = NULL;
	errCode = avformat_open_input(&stream_format_context, T2A(stream_url), NULL, &options);
	if (errCode < 0)
		return errCode;

	errCode = avformat_find_stream_info(stream_format_context, NULL);
	if (errCode < 0)
		return errCode;

	//////////////////////////////////////////////////////////////////////////
	//
	// �������������
	//
	//////////////////////////////////////////////////////////////////////////

	AVFormatContext *output_format_context = NULL;
	errCode = avformat_alloc_output_context2(&output_format_context, NULL, NULL, T2A(local_url));
	if (errCode < 0)
		return errCode;

	//////////////////////////////////////////////////////////////////////////
	//
	// ���������õ�����Ƶ��Ϣ
	//
	//////////////////////////////////////////////////////////////////////////

	int stream_video_index = -1;
	int stream_audio_index = -1;

	AVStream *input_video_stream = NULL;
	AVStream *output_video_stream = NULL;

	AVStream *input_audio_stream = NULL;
	AVStream *output_audio_stream = NULL;

	for (int index = 0; index < stream_format_context->nb_streams; ++index)
	{
		if (stream_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			stream_video_index = index;
			input_video_stream = stream_format_context->streams[index];
			output_video_stream = avformat_new_stream(output_format_context, input_video_stream->codec->codec);
		}
		else if (stream_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			stream_audio_index = index;
			input_audio_stream = stream_format_context->streams[index];
			output_audio_stream = avformat_new_stream(output_format_context, input_audio_stream->codec->codec);
		}
	}

	av_dump_format(stream_format_context, 0, T2A(stream_url), 0);

	

	//////////////////////////////////////////////////////////////////////////
	//
	// ������Դ
	//
	//////////////////////////////////////////////////////////////////////////

	system("pause");
	return 0;
}

