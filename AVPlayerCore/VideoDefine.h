#pragma once
#include <Windows.h>

enum class PLAY_MODE {
	PANO2D = 0,									// ��ͨ2D����ģʽ
	STANDARD = 1,									// ����ȫ������ģʽ
	FISH_EYE = 2,									// ���۲���ģʽ
	STEREO = 3,									// С���ǲ���ģʽ
};

enum class FRAME_FORMAT {
	VIDEO = 0,									// ��Ƶ
	PHOTO = 1,									// ͼƬ
};

enum class STREAM_FORMAT {
	FRAME_FORMAT_UNKNOWN = 0,						// UNKNOWN
	FRAME_FORMAT_YUYV = 1,						// ������YUV420
	FRAME_FORMAT_YUV = 2,						// ����Y-U-V	
};

typedef struct _PANO_INOF {
	const char*		windowTitle;						// ��������
	char*			className;							// ������������
	int				channels;							// �ӿ�����
	int				textureWidth;						// �����
	int				textureHeight;						// �����
	int				windowPosX;							// �������Ͻ�x����
	int				windowPosY;							// �������Ͻ�y����
	int				windowWidth;						// ���ڿ��
	int				windowHeight;						// ���ڸ߶�
	const char*		filePath;							// ͼƬ·��
	PLAY_MODE		playMode;							// ����ģʽ
	FRAME_FORMAT	format;								// ��Ƶ/ͼƬ��ʶ
} PANO_INFO;