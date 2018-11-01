// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // 从 Windows 头中排除极少使用的资料
// Windows 头文件: 
#include <windows.h>
#include <D3D9Types.h>

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

#include <atomic>
#include <string>
#include <mutex>
#include <thread>
#include <functional>
#include <Objbase.h>
#pragma comment(lib, "Ole32.lib")

#define STRINGIFY(A)					#A

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

inline std::string CreateGuidToString(char *str)
{
	GUID guid;
	CoCreateGuid(&guid);
	char buf[MAX_PATH] = { 0 };
	_snprintf_s(
		buf,
		sizeof(buf),
		"{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
		guid.Data1, guid.Data2, guid.Data3,
		guid.Data4[0], guid.Data4[1],
		guid.Data4[2], guid.Data4[3],
		guid.Data4[4], guid.Data4[5],
		guid.Data4[6], guid.Data4[7]);

	std::string strTemp;
	if (str)
		strTemp.append(str);

	strTemp.append(buf);
	return strTemp;
}