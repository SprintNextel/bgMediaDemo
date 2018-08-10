// GMVideoCollectionV2.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"

#include <atlconv.h>

#define __STDC_CONSTANT_MACROS

#ifdef __cplusplus
extern "C" {
#endif
#include "libavformat/avformat.h"
#include "libavcodec/avcodec.h"
#include "libavdevice/avdevice.h"
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

#include "SDL.h"
#include <iostream>

#include <windows.h>  
#include <vector>  
#include <dshow.h>  

#ifndef MACRO_GROUP_DEVICENAME  
#define MACRO_GROUP_DEVICENAME  

#define MAX_FRIENDLY_NAME_LENGTH    128  
#define MAX_MONIKER_NAME_LENGTH     256  

typedef struct _TDeviceName  
{  
	WCHAR FriendlyName[MAX_FRIENDLY_NAME_LENGTH];   // �豸�Ѻ���  
	WCHAR MonikerName[MAX_MONIKER_NAME_LENGTH];     // �豸Moniker��  
} TDeviceName;  
#endif

#pragma comment(lib, "Strmiids.lib")  

HRESULT DS_GetAudioVideoInputDevices( std::vector<TDeviceName> &vectorDevices, REFGUID guidValue )  
{  
	TDeviceName name;   
	HRESULT hr;  

	// ��ʼ��  
	vectorDevices.clear();  

	// ��ʼ��COM  
	hr = CoInitializeEx( NULL, COINIT_APARTMENTTHREADED );  
	if (FAILED(hr))  
	{  
		return hr;  
	}  

	// ����ϵͳ�豸ö����ʵ��  
	ICreateDevEnum *pSysDevEnum = NULL;  
	hr = CoCreateInstance( CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER, IID_ICreateDevEnum, (void **)&pSysDevEnum );  
	if (FAILED(hr))  
	{  
		CoUninitialize();  
		return hr;  
	}  

	// ��ȡ�豸��ö����  
	IEnumMoniker *pEnumCat = NULL;  
	hr = pSysDevEnum->CreateClassEnumerator( guidValue, &pEnumCat, 0 );  
	if (hr == S_OK)   
	{  
		// ö���豸����  
		IMoniker *pMoniker = NULL;  
		ULONG cFetched;  
		while(pEnumCat->Next( 1, &pMoniker, &cFetched ) == S_OK)  
		{  
			IPropertyBag *pPropBag;  
			hr = pMoniker->BindToStorage( NULL, NULL, IID_IPropertyBag, (void **)&pPropBag );  
			if (SUCCEEDED(hr))  
			{  
				// ��ȡ�豸�Ѻ���  
				VARIANT varName;  
				VariantInit( &varName );  

				hr = pPropBag->Read( L"FriendlyName", &varName, NULL );  
				if (SUCCEEDED(hr))  
				{  
					StringCchCopy( name.FriendlyName, MAX_FRIENDLY_NAME_LENGTH, varName.bstrVal );  

					// ��ȡ�豸Moniker��  
					LPOLESTR pOleDisplayName = reinterpret_cast<LPOLESTR>(CoTaskMemAlloc(MAX_MONIKER_NAME_LENGTH * 2));  
					if (pOleDisplayName != NULL)  
					{  
						hr = pMoniker->GetDisplayName( NULL, NULL, &pOleDisplayName );  
						if (SUCCEEDED(hr))  
						{  
							StringCchCopy( name.MonikerName, MAX_MONIKER_NAME_LENGTH, pOleDisplayName );  
							vectorDevices.push_back( name );  
						}  

						CoTaskMemFree( pOleDisplayName );  
					}  
				}  

				VariantClear( &varName );  
				pPropBag->Release();                       
			}  

			pMoniker->Release();  
		} // End for While  

		pEnumCat->Release();  
	}  

	pSysDevEnum->Release();  
	CoUninitialize();  

	return hr;  
}  


int _tmain(int argc, _TCHAR* argv[])
{
	av_register_all();
	avformat_network_init();
	avdevice_register_all();
	avcodec_register_all();

	//////////////////////////////////////////////////////////////////////////
	// ö������ͷ
	//AVFormatContext *pFormatCtx = avformat_alloc_context();  
	//AVDictionary* options = NULL;  
	//av_dict_set(&options, "list_devices", "true", 0);  
	//AVInputFormat *iformat = av_find_input_format("dshow");  
	//printf("Device Info=============\n");
	//AVInputFormat *input_fmt = iformat;
	//while (input_fmt != NULL)
	//{
	//	printf("��ƣ�%s\nȫ�ƣ�%s\n**********************\n", input_fmt->name, input_fmt->long_name);
	//	input_fmt = input_fmt->next;
	//}
	//avformat_open_input(&pFormatCtx, "video=dummy", iformat, &options);  
	//printf("========================\n");
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	//
	std::vector<TDeviceName> video_devices;
	DS_GetAudioVideoInputDevices(video_devices, CLSID_VideoInputDeviceCategory);

	std::vector<TDeviceName> audio_devices;
	DS_GetAudioVideoInputDevices(audio_devices, CLSID_AudioInputDeviceCategory);
	//////////////////////////////////////////////////////////////////////////

	//////////////////////////////////////////////////////////////////////////
	// ʹ��ffmpeg��ָ��������ͷ�豸
	AVFormatContext *pFormatCtx = avformat_alloc_context();  
	//AVDictionary* options = NULL;  
	//av_dict_set(&options, "list_options", "true", 0);  
	AVInputFormat *iformat = av_find_input_format("dshow");  
	char buffer[128] = {0};

	char err[AV_ERROR_MAX_STRING_SIZE] = {0};
	char *strerr = NULL;
	USES_CONVERSION;
	sprintf(buffer, "video=%s", W2A(video_devices[0].FriendlyName));  
	int errCode = avformat_open_input(&pFormatCtx, buffer, iformat, NULL); 
	if (errCode < 0)
	{
		strerr = av_make_error_string(err, AV_ERROR_MAX_STRING_SIZE, errCode);
	}
	//////////////////////////////////////////////////////////////////////////

	errCode = avformat_find_stream_info(pFormatCtx, NULL);

	int video_index = -1;
	AVStream *video_stream = NULL;
	AVCodec *video_codec = NULL;
	AVCodecContext *video_codec_ctx = NULL;

	int streams = pFormatCtx->nb_streams;
	for (int index = 0; index < streams; ++index)
	{
		if (pFormatCtx->streams[index]->codec->codec_type == AVMEDIA_TYPE_VIDEO)
		{
			// ��Ƶ����Ϣ
			video_index = index;
			video_stream = pFormatCtx->streams[index];

			video_codec = avcodec_find_decoder(video_stream->codec->codec_id);
			if (video_codec == NULL)
			{
				printf("û���ҵ�������\n");
				break;
			}

			video_codec_ctx = avcodec_alloc_context3(video_codec);
			if (video_codec_ctx == NULL)
			{
				printf("���������������ʧ�ܣ�");
				break;
			}

			errCode = avcodec_copy_context(video_codec_ctx, video_stream->codec);
			if (errCode < 0)
			{
				printf("���ƽ���������ʧ�ܣ������룺%d\n", errCode);
				break;
			}

			errCode = avcodec_open2(video_codec_ctx, video_codec, NULL);
			if (errCode < 0)
			{
				printf("�򿪽�����ʧ�ܣ������룺%d\n", errCode);
				break;
			}
		}
		else if (pFormatCtx->streams[index]->codec->codec_type == AVMEDIA_TYPE_AUDIO)
		{
			// ��Ƶ����Ϣ
		}
	}

	if (errCode < 0)
	{
		return errCode;
	}

	// ��������Ҫǿ��תΪH.264����H.265�������MPEG-4
	AVCodec *video_encoder = avcodec_find_encoder(AV_CODEC_ID_H264);
	if (video_encoder == NULL)
	{
		printf("δ�ҵ�������\n");
		return -1;
	}

	AVCodecContext *video_encode_ctx = avcodec_alloc_context3(video_encoder);
	if (video_encode_ctx == NULL)
	{
		printf("�������������ʧ�ܣ�\n");
		return -2;
	}

	// ��������Ԫ��
	video_encode_ctx->codec_type = AVMEDIA_TYPE_VIDEO;		// ý������
	//video_encode_ctx->bit_rate = 600000;					// ��Ƶ����
	video_encode_ctx->compression_level = 5;				// ѹ���ȼ�
	video_encode_ctx->width = video_codec_ctx->width;		// ��Ƶ��
	video_encode_ctx->height = video_codec_ctx->height;		// ��Ƶ��
	video_encode_ctx->pix_fmt = video_codec_ctx->pix_fmt;	// ͼ���ʽ
	//video_encode_ctx->time_base = av_inv_q(video_codec_ctx->framerate);

	// ��������ȡ
	while (true)
	{
		AVPacket pkt;
		errCode = av_read_frame(pFormatCtx, &pkt);
		if (errCode < 0)
			break;

		if (pkt.stream_index == video_index)
		{
			// ����Ƶ�����Ƚ��룬�ٱ���
			AVFrame *frm = av_frame_alloc();
			int got = 0;
			errCode = avcodec_decode_video2(video_codec_ctx, frm, &got, &pkt);
			if (!got)
			{
				printf("û�н����ͼƬ");
				av_frame_free(&frm);
				continue;
			}
		
			// ���±���
			int new_got = 0;
			AVPacket new_pkt;
			new_pkt.data = NULL;
			new_pkt.size = 0;
			av_init_packet(&new_pkt);
			errCode = avcodec_encode_video2(video_encode_ctx, &new_pkt, frm, &new_got);
			if (!new_got)
			{
				printf("����ʧ�ܣ�");
				av_frame_free(&frm);
				continue;
			}

			// ���Ҫ������ļ�����Ҫ������ID
			// ��Ҫ����ʱ���
		}
	}

	return 0;
}

