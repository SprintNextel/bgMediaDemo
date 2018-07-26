#ifndef _GxxGmAudio_H_
#define _GxxGmAudio_H_

#include "GxxGmQueue.h"

struct AVStream;
struct AVCodecContext;

/**
 * ������Ƶ���ҵ��
 */
class GxxGmAudio
{
public:
	GxxGmAudio();
	GxxGmAudio(AVCodecContext *audio_ctx, int audio_stream_index);
	~GxxGmAudio();

public:
	void SetStream(AVStream *stream);
	bool AudioPlay();

public:
	//int audio_decode_frame(AudioState *audio_state, uint8_t *audio_buf, int buf_size);
	//static void audio_callback(void* userdata, Uint8 *stream, int len);

private:
	const unsigned int buffer_size_;

	//GxxGmPacketQueue audioq_;			// ��Ƶ���������

	int audio_stream_index_;
	AVStream *audio_stream_;			// ��Ƶ��
	AVCodecContext *audio_ctx_;

	double audio_clock_;				// ��Ƶ��ʱ��

	unsigned char *audio_buff_;			// ��������ݵĻ���ռ�
	unsigned int audio_buff_size_;		// buffer�е��ֽ���
	unsigned int audio_buff_index_;		// buffer��δ�������ݵ�index
};

#endif//_GxxGmAudio_H_
