#include "GMAudioTranscode.h"

#define __STDC_CONSTANT_MACROS

extern "C" {
#include "libavformat/avformat.h"
#include "libavformat/avio.h"
#include "libavcodec/avcodec.h"
#include "libavutil/audio_fifo.h"
#include "libavutil/avassert.h"
#include "libavutil/avstring.h"
#include "libavutil/frame.h"
#include "libavutil/opt.h"
#include "libswresample/swresample.h"
};


// ����ļ�������
#define OUTPUT_BIT_RATE 96000
// ����ļ���������
#define OUTPUT_CHANNELS 2

