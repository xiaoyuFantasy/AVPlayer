// 下列 ifdef 块是创建使从 DLL 导出更简单的
// 宏的标准方法。此 DLL 中的所有文件都是用命令行上定义的 AVPLAYERCORE_EXPORTS
// 符号编译的。在使用此 DLL 的
// 任何其他项目上不应定义此符号。这样，源文件中包含此文件的任何其他项目都会将
// AVPLAYERCORE_API 函数视为是从 DLL 导入的，而此 DLL 则将用此宏定义的
// 符号视为是被导出的。
#ifdef AVPLAYERCORE_EXPORTS
#define PLAYERCORE_API __declspec(dllexport)
#else
#define PLAYERCORE_API __declspec(dllimport)
#endif

#include "AVPlayerDefine.h"
#include "VideoDefine.h"

#ifdef __cplusplus
extern "C" {
#endif
	/**
	* 功能描述	: 初始化播放器环境
	* 返回值	: 无
	*/
	void PLAYERCORE_API InitPlayer();

	/**
	* 功能描述	: 反初始化播放器环境
	* 返回值	: 无
	*/
	void PLAYERCORE_API UnInitPlayer();

	/**
	* 功能描述	: 创建播放器
	* 返回值	: 播放器句柄
	*/
	HANDLE  PLAYERCORE_API CreateMPlayer();

	/**
	* 功能描述	: 销毁播放器
	* 参数： 播放器句柄
	* 返回值	: 无
	*/
	void PLAYERCORE_API DestoryMPlayer(HANDLE handle);

	/**
	* 功能描述	: 配置
	* 返回值	: 无
	*/
	//void PLAYERCORE_API SetOptions(HANDLE handle, PLAYER_OPTIONS &opts);

	/**
	* 功能描述	: 设置声音
	* 返回值	: 无
	*/
	void PLAYERCORE_API SetVolume(HANDLE handle, int nVolume);

	/**
	* 功能描述	: 获取声音
	* 返回值	: 无
	*/
	int PLAYERCORE_API GetVolume(HANDLE handle);

	/**
	* 功能描述	: 配置
	* 返回值	: 无
	*/
	void PLAYERCORE_API SetMuted(HANDLE handle, bool bMuted);

	/**
	* 功能描述	: 配置
	* 返回值	: 无
	*/
	bool PLAYERCORE_API IsMuted(HANDLE handle);

	/**
	* 功能描述	: 全景模式
	* 返回值	: 无
	*/
	void PLAYERCORE_API SetPanoType(HANDLE handle, PLAY_MODE type);

	/**
	* 功能描述	: 全景缩放
	* 返回值	: 无
	*/
	void PLAYERCORE_API SetPanoScale(HANDLE handle, float factor);

	/**
	* 功能描述	: 全景旋转
	* 返回值	: 无
	*/
	void PLAYERCORE_API SetPanoRotate(HANDLE handle, float x, float y);

	/**
	* 功能描述	: 全景小行星旋转
	* 返回值	: 无
	*/
	void PLAYERCORE_API SetPanoScroll(HANDLE handle, float latitude, float longitude);

	/**
	* 功能描述	: 播放
	* 返回值	: 成功 or 失败
	*/
	bool PLAYERCORE_API Play(HANDLE handle, PLAYER_OPTS & opts);

	/**
	* 功能描述	: 停止播放
	* 返回值	: 成功 or 失败
	*/
	bool PLAYERCORE_API Stop(HANDLE handle);

	/**
	* 功能描述	: 是否正在播放
	* 返回值	: 是否
	*/
	bool PLAYERCORE_API IsPlaying(HANDLE handle);

	/**
	* 功能描述	: 暂停播放
	* 返回值	: 是否
	*/
	void PLAYERCORE_API Pause(HANDLE handle, bool bPause = true);

	/**
	* 功能描述	: 是否暂停播放
	* 返回值	: 是否
	*/
	bool PLAYERCORE_API IsPaused(HANDLE handle);

	/**
	* 功能描述	: 跳转
	* 返回值	: 是否
	*/
	void PLAYERCORE_API Seek(HANDLE handle, int nPos);

	/**
	* 功能描述	: 设置播放器大小
	* 返回值	: 无
	*/
	void PLAYERCORE_API SetWindowSize(HANDLE handle, int nWidth, int nHeight);

	/**
	* 功能描述	: 关闭Console
	* 返回值	: 无
	*/
	void PLAYERCORE_API OpenConsole(bool bOpen = true);

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif


