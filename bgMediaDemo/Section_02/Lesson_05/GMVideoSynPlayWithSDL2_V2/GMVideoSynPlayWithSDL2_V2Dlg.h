
// GMVideoSynPlayWithSDL2_V2Dlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"
#include <queue>

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavutil/avutil.h"
#include "libswresample/swresample.h"
#include "libswscale/swscale.h"
#include "libavutil/imgutils.h"
#ifdef __cplusplus
};
#endif

#include "SDL.h"
#include "afxcmn.h"

#define MAX_AUDIO_FRAME_SIZE 192000

class PacketQueue
{
public:
	PacketQueue()
	{
		nb_packets_ = 0;
		size_ = 0;

		mutex_ = SDL_CreateMutex();
		cond_ = SDL_CreateCond();
	}

	~PacketQueue()
	{
		SDL_DestroyMutex(mutex_);
		SDL_DestroyCond(cond_);

		mutex_ = NULL;
		cond_ = NULL;

		nb_packets_ = 0;
		size_ = 0;
	}

public:
	std::queue<AVPacket> queue_;

	unsigned int nb_packets_;
	unsigned int size_;
	SDL_mutex *mutex_;
	SDL_cond *cond_;

public:
	bool push(const AVPacket *packet)
	{
		AVPacket pkt;
		int errCode = av_packet_ref(&pkt, packet);
		if (errCode < 0)
		{
			char msg[AV_ERROR_MAX_STRING_SIZE] = {0};
			av_make_error_string(msg, AV_ERROR_MAX_STRING_SIZE, errCode);
			return false;
		}

		SDL_LockMutex(mutex_);
		queue_.push(pkt);

		size_ += pkt.size;
		++nb_packets_;

		SDL_CondSignal(cond_);
		SDL_UnlockMutex(mutex_);

		return true;
	}

	bool pop(AVPacket **packet, bool block)
	{
		bool ret = false;

		SDL_LockMutex(mutex_);

		while (true)
		{
			// ������һ��ȫ�ֿ�����������Ƿ��˳�
			if (!queue_.empty())
			{
				*packet = av_packet_clone(&queue_.front());
				//int errCode = av_packet_ref(*packet, &queue_.front());
				if (*packet == NULL)
				{
					//char msg[AV_ERROR_MAX_STRING_SIZE] = {0};
					//av_make_error_string(msg, AV_ERROR_MAX_STRING_SIZE, errCode);
					ret = false;
					break;
				}

				queue_.pop();
				--nb_packets_;
				size_ -= (*packet)->size;

				ret = true;
				break;
			}
			else if (!block)
			{
				ret = false;
				break;
			}
			else
				SDL_CondWait(cond_, mutex_);
		}

		SDL_UnlockMutex(mutex_);
		return ret;
	}

	void cleanup()
	{
		// ����򵥴ֱ�һ�㣬ֱ����գ�������ν����ʲô����
		// ��...����Ҫ�����ģ���һ���ĸ��쵰�߳���ȡ�������أ�

		SDL_LockMutex(mutex_);

		while (!queue_.empty())
		{
			queue_.pop();
		}

		SDL_UnlockMutex(mutex_);
	}

};


// CGMVideoSynPlayWithSDL2_V2Dlg �Ի���
class CGMVideoSynPlayWithSDL2_V2Dlg : public CDialog
{
// ����
public:
	CGMVideoSynPlayWithSDL2_V2Dlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_GMVIDEOSYNPLAYWITHSDL2_V2_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	int progress_pos_;	// ���Ž���
	int total_played_duration_;

	int volume_val_;	// ��������
	bool sound_switch_;	// ��������
	int volume_store_;	// �ر�������ʱ�򻺴���������������ʱ��ָ���ʱ��ȡ������

public:
	AVFormatContext *input_fmtctx_;

	// ý��������
	int video_stream_index_;
	int audio_stream_index_;
	int subtitle_stream_index_;

	// ý����
	AVStream *video_stream_;
	AVStream *audio_stream_;
	AVStream *subtitle_stream_;

	// �����������
	AVCodecContext *video_codec_ctx_;
	AVCodecContext *audio_codec_ctx_;
	AVCodecContext *subtitle_codec_ctx_;

	// ������
	AVCodec *video_codec_;
	AVCodec *audio_codec_;
	AVCodec *subtitle_codec_;

	// ����Ƶ���ı�����֡����
	PacketQueue video_packet_queue_;
	PacketQueue audio_packet_queue_;
	PacketQueue subtitle_packet_queue_;

public:
	static DWORD WINAPI VideoFrameRateControlThread(LPVOID lpParam);
	static DWORD WINAPI VideoDecodeThread(LPVOID lpParam);
	static DWORD WINAPI AudioDecodeThread(LPVOID lpParam);
	static DWORD WINAPI WorkingThread(LPVOID lpParam);
	static DWORD WINAPI StopManagerThread(LPVOID lpParam);

public:
	float framerate_;
	AVFrame *video_frame_yuv_;
	struct SwsContext *image_convert_context_;

	SwrContext *audio_convert_context_;
	unsigned char *audio_output_buffer_;
	int audio_output_buffer_size_;

public:
	SDL_Window *screen_;
	SDL_Renderer *screen_renderer_;
	SDL_Texture *screen_texture_;

	int screen_width_;
	int screen_height_;

	__int64 current_video_pts_;
	HANDLE framerate_event_;
	HANDLE pause_event_;
	bool is_paused_;

	static void SDLCALL audio_fill_callback(void *userdata, Uint8 * stream, int len);

public:
	void SetInfo(const char *info);

public:
	HANDLE Handle_VideoFrameRateControlThread_;
	HANDLE Handle_VideoDecodeThread_;
	HANDLE Handle_AudioDecodeThread_;
	HANDLE Handle_WorkingThread_;

public:
	CEdit m_cUrl;
	CEdit m_cInfo;

public:
	afx_msg void OnBnClickedBtnBrowse();
	afx_msg void OnBnClickedBtnDemuxing();
	afx_msg void OnBnClickedBtnPlay();
	afx_msg void OnBnClickedBtnPause();
	afx_msg void OnBnClickedBtnResume();
	afx_msg void OnBnClickedBtnStop();
	afx_msg void OnBnClickedCheckVolume();
	
	CButton m_cVolumeSwitch;
	CSliderCtrl m_cVolumeValue;
	
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	CProgressCtrl m_cProgressTime;
};
