// GMVideoDecode.cpp : �������̨Ӧ�ó������ڵ㡣
//
// ��������FLV��Ƶ�ļ�����ΪYUV�ļ���PCM�ļ�

//
// 2017-11-15 ����
// 1. yuv�ļ�ʹ��yuv��������ǿ�ܲ�����ɫ��̫�ԣ�֡�ʿ��Ƶ�Ҳ��̫��
// 2. pcm�ļ�����ʱһ����Ϳ
//

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
#include "libavutil/imgutils.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

#include <iostream>

#define MAX_AUDIO_FRAME_SIZE 192000


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 4)
	{
		printf("GMVideoDecode.exe <video_in_path> <yuv_path> <pcm_path>\n");
		return 0;
	}

	// ��ɳ�ʼ��ע��
	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	TCHAR video_in_path[4096] = {0};
	_tcscpy_s(video_in_path, 4096, argv[1]);

	TCHAR yuv_out_path[4096] = {0};
	_tcscpy_s(yuv_out_path, 4096, argv[2]);

	TCHAR pcm_out_path[4096] = {0};
	_tcscpy_s(pcm_out_path, 4096, argv[3]);

	USES_CONVERSION;

	//////////////////////////////////////////////////////////////////////////
	//
	// �������ļ�
	//
	//////////////////////////////////////////////////////////////////////////

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
	// �򿪶�Ӧ�Ľ����ļ�
	//
	//////////////////////////////////////////////////////////////////////////

	FILE *yuv_file = NULL;
	FILE *pcm_file = NULL;
	
	if (input_video_stream_index >= 0)
	{
		yuv_file = fopen(T2A(yuv_out_path), "wb");
		if (!yuv_file)
		{
			printf("Open yuv output file failed...\n");
			return -2;
		}
	}
	
	if (input_audio_stream_index >= 0)
	{
		pcm_file = fopen(T2A(pcm_out_path), "wb");
		if (!pcm_file)
		{
			printf("Open pcm output file failed...\n");
			return -2;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// �ҵ���Ƶ����������
	//
	//////////////////////////////////////////////////////////////////////////
	
	if (input_video_stream_index >= 0)
	{
		AVCodec *input_video_decoder = avcodec_find_decoder(input_video_codec_context->codec_id);
		if (!input_video_decoder)
		{
			printf("Not found video decoder...\n");
			return -2;
		}

		errCode = avcodec_open2(input_video_codec_context, input_video_decoder, NULL);
		if (errCode != 0)
		{
			printf("Open video decoder failed...%d\n", errCode);
			return errCode;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// �ҵ���Ƶ����������
	//
	//////////////////////////////////////////////////////////////////////////

	if (input_audio_stream_index >= 0)
	{
		AVCodec *input_audio_decoder = avcodec_find_decoder(input_audio_codec_context->codec_id);
		if (!input_audio_decoder)
		{
			printf("Not found audio decoder...\n");
			return -2;
		}

		errCode = avcodec_open2(input_audio_codec_context, input_audio_decoder, NULL);
		if (errCode != 0)
		{
			printf("Open audio decoder failed...%d\n", errCode);
			return errCode;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// ׼����Ƶͼ��ת������ؽṹ�����ݣ��Լ���ʼ��
	//
	//////////////////////////////////////////////////////////////////////////

	AVFrame *video_frame = av_frame_alloc();
	AVFrame *video_frame_yuv = av_frame_alloc();

	int video_frame_buffer_size = av_image_get_buffer_size(AV_PIX_FMT_YUV420P, input_video_codec_context->width, input_video_codec_context->height, 1);
	unsigned char *video_frame_buffer = (unsigned char *)av_malloc(video_frame_buffer_size);

	av_image_fill_arrays(video_frame_yuv->data, video_frame_yuv->linesize, video_frame_buffer,
		AV_PIX_FMT_YUV420P, input_video_codec_context->width, input_video_codec_context->height, 1);

	SwsContext *image_convert_context = sws_getContext(input_video_codec_context->width, input_video_codec_context->height, input_video_codec_context->pix_fmt,
		input_video_codec_context->width, input_video_codec_context->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);
	
	//////////////////////////////////////////////////////////////////////////
	//
	// ׼����Ƶͼ��ת������ؽṹ�����ݣ��Լ���ʼ��
	//
	//////////////////////////////////////////////////////////////////////////



	//////////////////////////////////////////////////////////////////////////
	//
	// ��ȡý���ļ��еı����
	//
	//////////////////////////////////////////////////////////////////////////
	av_dump_format(input_format_context, 0, T2A(video_in_path), 0);

	int got_data = 0;
	AVPacket *av_packet = (AVPacket *)av_malloc(sizeof(AVPacket));
	while (av_read_frame(input_format_context, av_packet) >= 0)
	{
		if (av_packet->stream_index == input_video_stream_index)
		{
			errCode = avcodec_decode_video2(input_video_codec_context, video_frame, &got_data, av_packet);
			if (errCode < 0)
			{
				printf("Decode video packet failed...%d\n", errCode);
				return errCode;
			}

			if (got_data)
			{
				sws_scale(image_convert_context, (const unsigned char * const *)video_frame->data, video_frame->linesize, 0, video_frame->height, video_frame_yuv->data, video_frame_yuv->linesize);

				int y_size = input_video_codec_context->width * input_video_codec_context->height;
				fwrite(video_frame_yuv->data[0], 1, y_size, yuv_file);
				fwrite(video_frame_yuv->data[1], 1, y_size / 4, yuv_file);
				fwrite(video_frame_yuv->data[2], 1, y_size / 4, yuv_file);

				printf("Succeed to decode 1 frame!\n");
			}
		}
		else if (av_packet->stream_index == input_audio_stream_index)
		{
		}

		av_free_packet(av_packet);
	}

	//////////////////////////////////////////////////////////////////////////
	//
	// ˢ�½�������ʣ���֡
	//
	//////////////////////////////////////////////////////////////////////////

	while (true)
	{
		errCode = avcodec_decode_video2(input_video_codec_context, video_frame, &got_data, av_packet);
		if (errCode < 0)
			break;

		if (!got_data)
			break;

		sws_scale(image_convert_context, (const unsigned char * const *)video_frame->data, video_frame->linesize, 0, video_frame->height, video_frame_yuv->data, video_frame_yuv->linesize);

		int y_size = input_video_codec_context->width * input_video_codec_context->height;
		fwrite(video_frame_yuv->data[0], 1, y_size, yuv_file);
		fwrite(video_frame_yuv->data[1], 1, y_size / 4, yuv_file);
		fwrite(video_frame_yuv->data[2], 1, y_size / 4, yuv_file);

		printf("Flush Decoder: Succeed to decode 1 frame!\n");
	}

	sws_freeContext(image_convert_context);

	fclose(yuv_file);

	av_frame_free(&video_frame_yuv);
	av_frame_free(&video_frame);
	avcodec_close(input_video_codec_context);
	avformat_close_input(&input_format_context);

	return 0;
}

