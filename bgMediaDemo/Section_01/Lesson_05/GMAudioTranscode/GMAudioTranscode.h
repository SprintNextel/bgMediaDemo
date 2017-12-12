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

#include <string>

typedef void (__stdcall * _GMAudioTranscodeStateCallback)(int progress);
typedef void (__stdcall * _GMAudioTranscodeExceptionCallback)(int errcode, std::string errinfo);

class FFmpegAudioTranscode;

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
	void SetupStateCallbackFunc(_GMAudioTranscodeStateCallback func);
	void SetupExceptionCallbackFunc(_GMAudioTranscodeExceptionCallback func);

public:
	int __stdcall Transcode(std::string source_audio, std::string target_audio);

private:
	FFmpegAudioTranscode *audio_transcoder_;

private:
	_GMAudioTranscodeStateCallback state_callback_;
	_GMAudioTranscodeExceptionCallback except_callback_;

};



#endif//_GM_AUDIO_TRANSCODE_H_
