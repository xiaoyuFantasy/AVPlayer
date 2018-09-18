#pragma once
#include <Windows.h>
#include <string>
#include "../AVPlayerCore/AVPlayerDefine.h"

// 初始化
typedef void(*funcInit)();
// 反初始化
typedef void(*funcUnInit)();
// 创建
typedef HANDLE(*funcCreatePlayer)();
// 销毁
typedef void(*funcDestoryPlayer)(HANDLE handle);
// 打开一个媒体文件
typedef bool(*funcOpen)(HANDLE handle, PLAYER_OPTS &opts, bool bSync);
// 开始播放视频.
typedef bool(*funcPlay)(HANDLE handle);
// 暂停播放.
typedef void(*funcPause)(HANDLE handle);
// 继续播放.
typedef void(*funcResume)(HANDLE handle);
// 停止播放.
typedef void(*funcStop)(HANDLE handle);
// 设置模型
typedef void(*funcSetRenderMode)(HANDLE handle, RENDER_MODE mode);
// 播放状态
typedef PLAY_STATUS (*funcStatus)(HANDLE handle);
// 等待播放直到完成.
typedef bool(*funcWaitForCompletion)(HANDLE handle);
// 关闭媒体
typedef void(*funcClose)(HANDLE handle);
// seek到某个时间播放, 按视频时长的百分比.
typedef void(*funcSeekTo)(HANDLE handle, double fact);
// 设置声音音量大小.
typedef void(*funcVolume)(HANDLE handle, int nVolume);
// 静音设置.
typedef void(*funcMute)(HANDLE handle, bool s);
// 全屏切换.
typedef void(*funcFullScreen)(HANDLE handle, bool bfull);
// 返回当前播放时间.
typedef double(*funcCurrentPlayTime)(HANDLE handle);
// 当前播放视频的时长, 单位秒.
typedef double(*funcDuration)(HANDLE handle);
// 当前播放视频的宽高, 单位像素.
typedef void(*funcVideoSize)(HANDLE handle, int width, int height);
// 当前缓冲进度, 单位百分比.
typedef double(*funcBuffering)(HANDLE handle);
// 设置缩放
typedef void(*funcSetScale)(HANDLE handle, float);
// 设置旋转
typedef void(*funcSetRotate)(HANDLE handle, double, double);
// 设置滚动
typedef void (*funcSetScroll)(HANDLE handle, double, double);
