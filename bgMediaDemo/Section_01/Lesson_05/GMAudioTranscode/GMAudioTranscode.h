#ifndef _GM_AUDIO_TRANSCODE_H_
#define _GM_AUDIO_TRANSCODE_H_

//////////////////////////////////////////////////////////////////////////
//
// �ļ�˵����������Ƶת���ӿ�
// ʱ�䣺2017-12-11
// ���ߣ�wangyu
// �汾��v1.0.1
//
//////////////////////////////////////////////////////////////////////////

typedef void (__stdcall * _GMAudioTranscodeStateCallback)(int work_index, int progress);
typedef void (__stdcall * _GMAudioTranscodeExceptionCallback)(int work_index, int errcode, std::string errinfo);

/**
 * �ӿ�˵������ʼһ����Ƶת�빤��
 * ����˵����
 *	@source_audio		ԭʼ��Ƶ�ļ�ȫ·��
 *	@target_audio		ת�������Ƶ�ļ�ȫ·��
 *	@state_callback		ת��״̬�ص�֪ͨ����
 *	@except_callback	ת���쳣�ص�֪ͨ����
 */
class GMMediaTranscoder
{
public:
	GMMediaTranscoder();
	~GMMediaTranscoder();

public:
	int __stdcall GMAudioTranscode(std::string source_audio, std::string target_audio);

private:
	int open_input_file(const char *filename);
	int open_output_file(const char *filename);

private:
	_GMAudioTranscodeStateCallback state_callback_;
	_GMAudioTranscodeExceptionCallback except_callback_;

private:
	AVFormatContext **input_format_context;
	AVCodecContext **input_codec_context;
};



#endif//_GM_AUDIO_TRANSCODE_H_
