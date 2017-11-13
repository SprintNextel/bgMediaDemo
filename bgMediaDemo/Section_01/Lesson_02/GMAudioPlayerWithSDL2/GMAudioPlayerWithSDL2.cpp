// GMAudioPlayerWithSDL2.cpp : �������̨Ӧ�ó������ڵ㡣
//
// ����������ʹ��SDL2����PCM��Ƶ��������

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

#include "SDL.h"
#include <iostream>

#define MAX_AUDIO_FRAME_SIZE 192000


int audio_len = 0;
unsigned char *audio_pos = NULL;
unsigned char *audio_chunk = NULL;

void audio_fill_callback(void *udata, Uint8 *stream, int len)
{
	SDL_memset(stream, 0, len);

	if(audio_len == 0)
		return;

	len = (len > audio_len ? audio_len : len);

	SDL_MixAudio(stream, audio_pos, len, SDL_MIX_MAXVOLUME);
	audio_pos += len;
	audio_len -= len;
}


int _tmain(int argc, _TCHAR* argv[])
{
	if (argc < 2)
	{
		printf("GMAudioPlayerWithSDL2.exe <audio_pcm_path>\n");
		return 0;
	}

	// ��ɳ�ʼ��ע��
	av_register_all();
	avformat_network_init();
	avcodec_register_all();

	TCHAR audio_path[4096] = {0};
	_tcscpy_s(audio_path, 4096, argv[1]);

	int errCode = SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER);
	if (errCode < 0)
	{
		printf("Could not initialize SDL - %s\n", SDL_GetError());
		return errCode;
	}

	// ��Ȼ�Ƚ��н���ɣ���Ȼû�취��Ȼ��ȡ���Ų���
	USES_CONVERSION;

	// ����Ƶ�ļ�
	AVFormatContext *format_context = NULL;
	errCode = avformat_open_input(&format_context, T2A(audio_path), NULL, NULL);
	if (errCode != 0)
	{
		printf("avformat_open_input failed.\n");
		return errCode;
	}

	// ����������Ƶ��Ϣ
	avformat_find_stream_info(format_context, NULL);

	// ������Ƶ������
	int audio_stream_index = -1;
	for (int index = 0; index < format_context->nb_streams; ++index)
	{
		if (format_context->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			audio_stream_index = index;
			break;
		}
	}

	if (audio_stream_index < 0)
	{
		printf("There isn't a audio stream in this file ...\n");
		return -2;
	}

	//av_dump_format(format_context, 0, T2A(audio_path), 0);

	// �õ���Ƶ�����Լ�������������
	AVStream *audio_stream = format_context->streams[audio_stream_index];
	AVCodecContext *audio_codec_context = audio_stream->codec;


	// ���ݽ�����ID���ҽ�����������
	AVCodec *audio_codec = avcodec_find_decoder(audio_stream->codec->codec_id);
	if (audio_codec == NULL)
	{
		printf("Not found decoder ...\n");
		return -2;
	}

	errCode = avcodec_open2(audio_codec_context, audio_codec, NULL);
	if (errCode != 0)
	{
		printf("Open decoder failed ...\n");
		return -5;
	}

	// ׼������֮ǰ�����趨��PCMת����
	uint64_t output_channel_layout = AV_CH_LAYOUT_STEREO;
	int output_frame_size = audio_codec_context->frame_size;	// AAC��1024��MP3��1152��
	AVSampleFormat output_sample_format = AV_SAMPLE_FMT_S16;
	int output_sample_rate = audio_codec_context->sample_rate;	// 44100
	int output_channels = av_get_channel_layout_nb_channels(output_channel_layout);
	int output_buffer_size = av_samples_get_buffer_size(NULL, output_channels, output_frame_size, output_sample_format, 1);

	unsigned char *output_buffer = (unsigned char *)av_malloc(MAX_AUDIO_FRAME_SIZE * 2);

	// 
	int input_channel_layout = av_get_default_channel_layout(audio_codec_context->channels);

	// 
	SwrContext *audio_convert_context = swr_alloc();
	audio_convert_context = swr_alloc_set_opts(audio_convert_context, output_channel_layout, output_sample_format, output_sample_rate,
		input_channel_layout, audio_codec_context->sample_fmt, audio_codec_context->sample_rate, 0, NULL);
	swr_init(audio_convert_context);

	// SDL����Ƶ���Ų�����ʵ���ϸ������е��ϵ
	SDL_AudioSpec audio_spec;
	audio_spec.freq = output_sample_rate;
	audio_spec.format = AUDIO_S16SYS;
	audio_spec.channels = output_channels;
	audio_spec.silence = 0;
	audio_spec.samples = output_frame_size;
	audio_spec.callback = audio_fill_callback;

	// SDL����Ƶͨ��
	errCode = SDL_OpenAudio(&audio_spec, NULL);
	if (errCode < 0)
	{
		printf("Can't open audio...\n");
		return errCode;
	}

	// Play
	SDL_PauseAudio(0);

	// ��ȡ�������ִ�н���
	AVPacket av_packet;
	AVFrame *av_frame = av_frame_alloc();

	bool is_print_info = false;
	while (av_read_frame(format_context, &av_packet) == 0)
	{
		if (av_packet.stream_index != audio_stream_index)
		{
			// ������Ƶ�������еİ������ǷŹ�
			continue;
		}

		// ������Ƶ�����
		int got_frame_ptr = 0;
		errCode = avcodec_decode_audio4(audio_codec_context, av_frame, &got_frame_ptr, &av_packet);

		if (!got_frame_ptr)
			continue;

		// ִ��ת��
		swr_convert(audio_convert_context, &output_buffer, MAX_AUDIO_FRAME_SIZE, (const unsigned char **)av_frame->data, av_frame->nb_samples);

		while (audio_len > 0)
			SDL_Delay(1);

		audio_chunk = (unsigned char *)output_buffer;
		audio_len = output_buffer_size;
		audio_pos = audio_chunk;
	}

	SDL_CloseAudio();
	SDL_Quit();

	avcodec_close(audio_codec_context);
	av_free(output_buffer);
	swr_free(&audio_convert_context);
	av_frame_free(&av_frame);
	avformat_close_input(&format_context);

	system("pause");
	return 0;
}

