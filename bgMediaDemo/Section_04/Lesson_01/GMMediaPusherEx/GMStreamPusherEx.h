#ifndef _GM_STREAM_PUSHER_EX_H_
#define _GM_STREAM_PUSHER_EX_H_

#include <string>

struct AVFormatContext;
struct AVOutputFormat;
struct AVCodecContext;
struct AVCodec;
struct AVStream;

#define __STDC_CONSTANT_MACROS

// ��������
#define MEDIA_TYPE_VIDEO				0x00000001
#define MEDIA_TYPE_AUDIO				0x00000010

// ��Ƶ������ID
#define MEDIA_CODEC_H264				0x00000001
	
// ��Ƶ������ID
#define MEDIA_CODEC_AAC					0x00000001
#define MEDIA_CODEC_MP3					0x00000002
#define MEDIA_CODEC_G711u				0x00000003

// ͼ����ɫģʽ
#define VIDEO_COLOR_FMT_YUV420			0x00000001

// ��Ƶ������ʽ
#define AUDIO_CHENNAL_LAYOUT_MONO		0x00000001
#define AUDIO_CHENNAL_LAYOUT_STEREO		0x00000002

// ��Ƶλ��
#define AUDIO_BIT_U8					0x00000001
#define AUDIO_BIT_S16					0x00000002
#define AUDIO_BIT_S32					0x00000003
#define AUDIO_BIT_FLT					0x00000004
#define AUDIO_BIT_DBL					0x00000005

#define AUDIO_BIT_U8P					0x00000006
#define AUDIO_BIT_S16P					0x00000007
#define AUDIO_BIT_S32P					0x00000008
#define AUDIO_BIT_FLTP					0x00000009
#define AUDIO_BIT_DBLP					0x0000000A


class GMStreamPusherEx
{
public:
	GMStreamPusherEx();
	~GMStreamPusherEx();

public:
	int OpenTargetUrl(const char *target_url);

public:
	int SetVideoParam(int width, int height, int frame_rate, int video_codec = MEDIA_CODEC_H264, int color_fmt = VIDEO_COLOR_FMT_YUV420);
	int SetAudioParam(int bit_rate, int sample_rate, int sample_fmt = AUDIO_BIT_S16, int channel_layout = AUDIO_CHENNAL_LAYOUT_STEREO, int audio_codec = MEDIA_CODEC_AAC);

public:
	int StartPush();
	int InputMediaData(const unsigned char *data, int data_len, int data_type = MEDIA_TYPE_VIDEO);
	void StopPush();

private:
	std::string target_url_;
	AVFormatContext *output_format_context;
	AVOutputFormat *output_format;
	AVStream *output_video_stream;
	AVStream *output_audio_stream;
	AVCodecContext *output_video_codec_context;
	AVCodecContext *output_audio_codec_context;
	AVCodec *output_video_codec;
	AVCodec *output_audio_codec;
};

#endif//_GM_STREAM_PUSHER_EX_H_