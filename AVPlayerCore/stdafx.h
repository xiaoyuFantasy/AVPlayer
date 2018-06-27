// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // �� Windows ͷ���ų�����ʹ�õ�����
// Windows ͷ�ļ�: 
#include <windows.h>


// TODO:  �ڴ˴����ó�����Ҫ������ͷ�ļ�
extern "C" {
#include "libavcodec\avcodec.h"
#include "libavformat\avformat.h"
#include "libswscale\swscale.h"
#include "libswresample\swresample.h"
#include "libavdevice\avdevice.h"
#include "libavfilter\avfilter.h"
#include "libavutil\hwcontext.h"
#include "libavutil\imgutils.h"
#include "libavutil\time.h"
#include "SDL.h"
}

#include "Lock.h"
#include "Queue.h"

using PacketPtr = std::unique_ptr<AVPacket, std::function<void(AVPacket*)>>;
using FramePtr = std::unique_ptr<AVFrame, std::function<void(AVFrame*)>>;
using PacketQueue = CQueue<PacketPtr>;
using FrameQueue = CQueue<FramePtr>;

inline std::string err2str(int err)
{
	char szErr[MAXCHAR + 1] = { 0 };
	av_strerror(err, szErr, MAXCHAR);
	return szErr;
}