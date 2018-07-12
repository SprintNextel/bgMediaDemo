#ifndef _GMMediaCollection_H_
#define _GMMediaCollection_H_

/**
 * ���ඨ����һ��ý��ɼ����������ڲɼ����������ͷ����˷������Ƶ����
 * �����û��趨����ָ���������ʽ�����ݰ����
 *
 * ���У�
 * ��Ƶ���ݲ��ı�ֱ��ʣ���ɫģʽ��ֻ������ת��
 * ��Ƶ���ݴ���˷�ɼ������Ժ������ŵĲ����ʽ����ر���
 */

class GmGxxStreamNotifier
{
public:
	virtual int StreamData(enum AVMediaType type, AVFrame *) = 0;
};

class GMMediaCollection
{
public:
	struct GmGxxDeviceInfo
	{
		std::string name_;
		std::string long_name_;
	};

public:
	GMMediaCollection();
	~GMMediaCollection();

public:
	int EnumDeviceInfo(std::vector<GmGxxDeviceInfo> &video_devs, std::vector<GmGxxDeviceInfo> &audio_devs);

public:
	int StartVideo(enum AVCodecID encode_id);
	int StartAudio(enum AVCodecID encode_id);

	int StopVideo();
	int StopAudio();
};

#endif//_GMMediaCollection_H_
