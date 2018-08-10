#ifndef _GxxGmDecoderPlay_H_
#define _GxxGmDecoderPlay_H_

struct AVFormatContext;
struct AVStream;
struct AVCodecContext;
struct AVCodec;
//enum AVPixelFormat;

class GxxGmDecoderPlay
{
public:
	GxxGmDecoderPlay();
	~GxxGmDecoderPlay();

public:
	// ��ʼ��OpenGL����
	int Initialize(int screen);
	void Destroy();

public:
	int Demuxing(const char *url);

public:
	int Play();
	void Stop();

private:
	// �⸴�õõ�������
	AVFormatContext *input_fmtctx;
	
	int video_stream_index;
	int audio_stream_index;

	AVStream *video_stream;
	AVStream *audio_stream;

	AVCodecContext *video_codec_ctx;
	AVCodecContext *audio_codec_ctx;

	AVCodec *video_codec;
	AVCodec *audio_codec;

	float video_frame_rate;
	int video_width;
	int video_height;
	__int64 video_bitrate;
	//enum AVPixelFormat pix_fmt;
	int video_level;

private:
	// ͼ��ת���õ������ݺͽṹ
};

#endif//_GxxGmDecoderPlay_H_
