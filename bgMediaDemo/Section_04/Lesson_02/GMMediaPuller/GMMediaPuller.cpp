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

//#define USE_H264BSF
#define USE_AACBSF

bool need_stop = false;

BOOL WINAPI HandlerRoutine(DWORD dwCtrlType)
{
	switch (dwCtrlType)
	{
	case CTRL_C_EVENT:
		// Ctrl + C
		need_stop = true;
		break;
	case CTRL_BREAK_EVENT:
		// Ctrl + Break
		need_stop = true;
		break;
	case CTRL_CLOSE_EVENT:
		// Close or End Task
		need_stop = true;
		break;
	case CTRL_LOGOFF_EVENT:
		// System user logoff
		need_stop = true;
		break;
	case CTRL_SHUTDOWN_EVENT:
		// System will shutdown
		need_stop = true;
		break;
	default:
		break;
	}
	return TRUE;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 3)
	{
		printf("%s <stream_url> <local_url>\n", argv[0]);
		return 0;
	}

	BOOL b = SetConsoleCtrlHandler(HandlerRoutine, TRUE);

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

	AVOutputFormat *output_format = output_format_context->oformat;

	//////////////////////////////////////////////////////////////////////////
	//
	// ���������õ�����Ƶ��Ϣ
	//
	//////////////////////////////////////////////////////////////////////////

	int input_video_stream_index = -1;
	int input_audio_stream_index = -1;

	int output_video_stream_index = -1;
	int output_audio_stream_index = -1;

	AVStream *input_video_stream = NULL;
	AVStream *output_video_stream = NULL;

	AVStream *input_audio_stream = NULL;
	AVStream *output_audio_stream = NULL;

	for (int index = 0; index < stream_format_context->nb_streams; ++index)
	{
		if (stream_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			input_video_stream_index = index;
			input_video_stream = stream_format_context->streams[index];

			output_video_stream = avformat_new_stream(output_format_context, input_video_stream->codec->codec);

			if (!output_video_stream)
				return AVERROR_UNKNOWN;

			output_video_stream_index = output_video_stream->index;

			// ���Ʊ���������
			errCode = avcodec_copy_context(output_video_stream->codec, input_video_stream->codec);
			if (errCode < 0)
				return errCode;

			output_video_stream->codec->codec_tag = 0;

			if (output_format_context->oformat->flags & AVFMT_GLOBALHEADER)
				output_video_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		}
		else if (stream_format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			input_audio_stream_index = index;
			input_audio_stream = stream_format_context->streams[index];

			output_audio_stream = avformat_new_stream(output_format_context, input_audio_stream->codec->codec);

			if (!output_audio_stream)
				return AVERROR_UNKNOWN;

			output_audio_stream_index = output_audio_stream->index;

			// ���Ʊ���������
			errCode = avcodec_copy_context(output_audio_stream->codec, input_audio_stream->codec);
			if (errCode < 0)
				return errCode;

			output_audio_stream->codec->codec_tag = 0;

			if (output_format_context->oformat->flags & AVFMT_GLOBALHEADER)
				output_audio_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
		}
	}

	av_dump_format(stream_format_context, 0, T2A(stream_url), 0);

	//////////////////////////////////////////////////////////////////////////
	//
	// ������ļ���д���ļ�ͷ
	//
	//////////////////////////////////////////////////////////////////////////

	if (!(output_format_context->flags & AVFMT_NOFILE))
	{
		errCode = avio_open(&output_format_context->pb, T2A(local_url), AVIO_FLAG_WRITE);
		if (errCode < 0)
			return errCode;
	}

	errCode = avformat_write_header(output_format_context, NULL);
	if (errCode < 0)
		return errCode;

#ifdef USE_H264BSF
	AVBitStreamFilterContext *h264_bit_stream_filter_context = av_bitstream_filter_init("h264_mp4toannexb");
#endif

#ifdef USE_AACBSF
	AVBitStreamFilterContext *aac_bit_stream_filter_context = av_bitstream_filter_init("aac_adtstoasc");
#endif

	AVPacket av_packet;
	int frame_index = 0;
	int total = 0;
	int size = 0;
	int stream_index = -1;

	// ����һ����Ƶ֡�������Ƶ֡�ķ�ʽ���и��ã�����һ��
	while (true)
	{
		if (need_stop)
			break;

		errCode = av_read_frame(stream_format_context, &av_packet);
		if (errCode < 0)
			break;

		if (av_packet.stream_index == input_video_stream_index)
		{
#ifdef USE_H264BSF
			av_bitstream_filter_filter(h264_bit_stream_filter_context, input_video_stream->codec, NULL, &av_packet.data, &av_packet.size, av_packet.data, av_packet.size, 0);
#endif
			stream_index = output_video_stream_index;
		}
		else if (av_packet.stream_index == input_audio_stream_index)
		{
#ifdef USE_AACBSF
			av_bitstream_filter_filter(aac_bit_stream_filter_context, input_audio_stream->codec, NULL, &av_packet.data, &av_packet.size, av_packet.data, av_packet.size, 0);
#endif
			stream_index = output_audio_stream_index;
		}

		// ת��PTS/DTS
		av_packet.pts = av_rescale_q_rnd(av_packet.pts, input_video_stream->time_base, output_video_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		av_packet.dts = av_rescale_q_rnd(av_packet.dts, input_video_stream->time_base, output_video_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF|AV_ROUND_PASS_MINMAX));
		av_packet.duration = av_rescale_q(av_packet.duration, input_video_stream->time_base, output_video_stream->time_base);
		av_packet.pos = -1;
		av_packet.stream_index = stream_index;

		size = av_packet.size;

		// д������
		errCode = av_interleaved_write_frame(output_format_context, &av_packet);
		if (errCode < 0)
			break;

		printf("Write a  packet.\t size : %d,\t total : %d\n", size, total);
		total += size;

		av_free_packet(&av_packet);
	}

	// д�ļ�β
	av_write_trailer(output_format_context);
	printf("Write file tailer...\n");

	//////////////////////////////////////////////////////////////////////////
	//
	// ������Դ
	//
	//////////////////////////////////////////////////////////////////////////

	avformat_close_input(&stream_format_context);

	// �ر�����ļ�
	if (output_format_context && !(output_format->flags & AVFMT_NOFILE))
		avio_close(output_format_context->pb);

	avformat_free_context(output_format_context);

	system("pause");
	return 0;
}
