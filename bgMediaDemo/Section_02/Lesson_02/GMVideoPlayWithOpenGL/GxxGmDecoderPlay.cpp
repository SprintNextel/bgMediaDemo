#include "stdafx.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libswscale/swscale.h"
#ifdef __cplusplus
};
#endif

#include "glew.h"
#include "glut.h"

#include "GxxGmDecoderPlay.h"



GxxGmDecoderPlay::GxxGmDecoderPlay()
: input_fmtctx(NULL)
, video_stream_index(-1)
, audio_stream_index(-1)
, video_stream(NULL)
, audio_stream(NULL)
, video_codec_ctx(NULL)
, audio_codec_ctx(NULL)
, video_codec(NULL)
, audio_codec(NULL)
{

}

GxxGmDecoderPlay::~GxxGmDecoderPlay()
{

}

int GxxGmDecoderPlay::Initialize(int screen)
{
	int errCode = 0;

	RECT r;
	GetWindowRect((HWND)screen, &r);
	int screen_width = r.right - r.left;
	int screen_height = r.bottom - r.top;

	// GLUT init
	glutInit(1, NULL);

	//Double, Use glutSwapBuffers() to show
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB );

	//Single, Use glFlush() to show
	//glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB );

	glutSetWindow(screen);

	return errCode;
}

int GxxGmDecoderPlay::Demuxing(const char *url)
{
	char msg[4096] = {0};
	int errCode = avformat_open_input(&input_fmtctx, url, NULL, NULL);
	if (errCode < 0)
	{
		av_make_error_string(msg, AV_ERROR_MAX_STRING_SIZE, errCode);
		return errCode;
	}

	for (int index = 0; index < input_fmtctx->nb_streams; ++index)
	{
		AVStream *current_stream = input_fmtctx->streams[index];
		if (current_stream->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			video_stream = current_stream;

			video_codec = avcodec_find_decoder(video_stream->codec->codec_id);
			if (video_codec == NULL)
			{
				// û���ҵ������������Ϊ�����˳�
				errCode = -2;
				break;
			}

			// ����һ�ݽ���������
			video_codec_ctx = avcodec_alloc_context3(video_codec);
			errCode = avcodec_copy_context(video_codec_ctx, video_stream->codec);
			if (errCode < 0)
			{
				av_make_error_string(msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				break;
			}

			// �򿪽�����
			errCode = avcodec_open2(video_codec_ctx, video_codec, NULL);
			if (errCode < 0)
			{
				av_make_error_string(msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				break;
			}

			// ��ȡ��Ƶ��������
			(float)video_stream->avg_frame_rate.num / (float)video_stream->avg_frame_rate.den;	// ֡��
			video_codec_ctx->width;		// ��Ƶ��
			video_codec_ctx->height;	// ��Ƶ��
			video_codec_ctx->bit_rate;	// ����
			video_codec_ctx->pix_fmt;	// ͼ���ʽ
			video_codec_ctx->level;
		}
		else if (current_stream->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream = current_stream;

			audio_codec = avcodec_find_decoder(audio_stream->codec->codec_id);
			if (video_codec == NULL)
			{
				// û���ҵ������������Ϊ�����˳�
				errCode = -2;
				break;
			}

			// ����һ�ݽ���������
			audio_codec_ctx = avcodec_alloc_context3(audio_codec);
			errCode = avcodec_copy_context(audio_codec_ctx, audio_stream->codec);
			if (errCode < 0)
			{
				av_make_error_string(msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				break;
			}

			// �򿪽�����
			errCode = avcodec_open2(audio_codec_ctx, audio_codec, NULL);
			if (errCode < 0)
			{
				av_make_error_string(msg, AV_ERROR_MAX_STRING_SIZE, errCode);
				break;
			}

			// ��ȡ��Ƶ��������
			audio_codec_ctx->sample_rate;
			audio_codec_ctx->bit_rate;
			audio_codec_ctx->channels;
			audio_codec_ctx->level;
			audio_codec_ctx->
		}
	}

	if (errCode != 0)
		return errCode;

	// ��ȡ
}