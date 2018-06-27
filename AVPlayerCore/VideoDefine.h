#pragma once
#include <Windows.h>

enum class PLAY_MODE {
	PANO2D = 0,									// 普通2D播放模式
	STANDARD = 1,									// 球形全景播放模式
	FISH_EYE = 2,									// 鱼眼播放模式
	STEREO = 3,									// 小行星播放模式
};

enum class FRAME_FORMAT {
	VIDEO = 0,									// 视频
	PHOTO = 1,									// 图片
};

enum class STREAM_FORMAT {
	FRAME_FORMAT_UNKNOWN = 0,						// UNKNOWN
	FRAME_FORMAT_YUYV = 1,						// 单独含YUV420
	FRAME_FORMAT_YUV = 2,						// 分量Y-U-V	
};

typedef struct _PANO_INOF {
	const char*		windowTitle;						// 窗口主题
	char*			className;							// 创建窗口类名
	int				channels;							// 视口数量
	int				textureWidth;						// 纹理宽
	int				textureHeight;						// 纹理高
	int				windowPosX;							// 窗口左上角x坐标
	int				windowPosY;							// 窗口左上角y坐标
	int				windowWidth;						// 窗口宽度
	int				windowHeight;						// 窗口高度
	const char*		filePath;							// 图片路径
	PLAY_MODE		playMode;							// 播放模式
	FRAME_FORMAT	format;								// 视频/图片标识
} PANO_INFO;