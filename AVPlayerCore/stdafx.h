// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>


// TODO:  在此处引用程序需要的其他头文件
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