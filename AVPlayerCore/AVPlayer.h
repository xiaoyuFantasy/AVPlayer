#pragma once
#ifdef AVPLAYERCORE_EXPORTS
#define EXPORT_API __declspec(dllexport)
#else
#define EXPORT_API __declspec(dllimport)
#endif

#include "AVPlayerDefine.h"
#include "VideoDefine.h"

#ifdef __cplusplus
extern "C" {
#endif
	EXPORT_API void  OpenConsole(bool bOpen);
	/**
	* 功能描述	: 初始化播放器环境
	* 返回值	: 无
	*/
	EXPORT_API void InitPlayer();

	/**
	* 功能描述	: 反初始化播放器环境
	* 返回值	: 无
	*/
	EXPORT_API void UnInitPlayer();

	/**
	* 功能描述	: 创建播放器
	* 返回值	: 播放器句柄
	*/
	EXPORT_API HANDLE CreateAVPlayer();

	/**
	* 功能描述	: 销毁播放器
	* 参数： 播放器句柄
	* 返回值	: 无
	*/
	EXPORT_API void DestoryAVPlayer(HANDLE handle);

	// 打开一个媒体文件
	EXPORT_API bool Open(HANDLE handle, PLAYER_OPTS &opts, bool bSync = true);

	// 开始播放视频.
	EXPORT_API bool Play(HANDLE handle);

	// 暂停播放.
	EXPORT_API void Pause(HANDLE handle);

	// 继续播放.
	EXPORT_API void Resume(HANDLE handle);

	// 停止播放.
	EXPORT_API void Stop(HANDLE handle);

	// 播放状态
	EXPORT_API PLAY_STATUS Status(HANDLE handle);

	// 等待播放直到完成.
	EXPORT_API bool WaitForCompletion(HANDLE handle);

	// 关闭媒体
	EXPORT_API void Close(HANDLE handle);

	// seek到某个时间播放, 按视频时长的百分比.
	EXPORT_API void SeekTo(HANDLE handle, double fact);

	// 设置声音音量大小.
	EXPORT_API void Volume(HANDLE handle, int nVolume);

	// 静音设置.
	EXPORT_API void Mute(HANDLE handle, bool s);

	// 全屏切换.
	EXPORT_API void FullScreen(HANDLE handle, bool bfull);

	// 返回当前播放时间.
	EXPORT_API double CurrentPlayTime(HANDLE handle);

	// 当前播放视频的时长, 单位秒.
	EXPORT_API double Duration(HANDLE handle);

	// 当前播放视频的宽高, 单位像素.
	EXPORT_API void VideoSize(HANDLE handle, int width, int height);

	// 当前缓冲进度, 单位百分比.
	EXPORT_API double Buffering(HANDLE handle);

	// 渲染模型
	EXPORT_API void SetRenderMode(HANDLE handle, RENDER_MODE mode);

	//缩放
	EXPORT_API void SetScale(HANDLE handle, float factor);

	// 滚动
	EXPORT_API void SetRotate(HANDLE handle, double xoffset, double yoffset);

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif
