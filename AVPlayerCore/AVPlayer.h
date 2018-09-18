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
	* ��������	: ��ʼ������������
	* ����ֵ	: ��
	*/
	EXPORT_API void InitPlayer();

	/**
	* ��������	: ����ʼ������������
	* ����ֵ	: ��
	*/
	EXPORT_API void UnInitPlayer();

	/**
	* ��������	: ����������
	* ����ֵ	: ���������
	*/
	EXPORT_API HANDLE CreateAVPlayer();

	/**
	* ��������	: ���ٲ�����
	* ������ ���������
	* ����ֵ	: ��
	*/
	EXPORT_API void DestoryAVPlayer(HANDLE handle);

	// ��һ��ý���ļ�
	EXPORT_API bool Open(HANDLE handle, PLAYER_OPTS &opts, bool bSync = true);

	// ��ʼ������Ƶ.
	EXPORT_API bool Play(HANDLE handle);

	// ��ͣ����.
	EXPORT_API void Pause(HANDLE handle);

	// ��������.
	EXPORT_API void Resume(HANDLE handle);

	// ֹͣ����.
	EXPORT_API void Stop(HANDLE handle);

	// ����״̬
	EXPORT_API PLAY_STATUS Status(HANDLE handle);

	// �ȴ�����ֱ�����.
	EXPORT_API bool WaitForCompletion(HANDLE handle);

	// �ر�ý��
	EXPORT_API void Close(HANDLE handle);

	// seek��ĳ��ʱ�䲥��, ����Ƶʱ���İٷֱ�.
	EXPORT_API void SeekTo(HANDLE handle, double fact);

	// ��������������С.
	EXPORT_API void Volume(HANDLE handle, int nVolume);

	// ��������.
	EXPORT_API void Mute(HANDLE handle, bool s);

	// ȫ���л�.
	EXPORT_API void FullScreen(HANDLE handle, bool bfull);

	// ���ص�ǰ����ʱ��.
	EXPORT_API double CurrentPlayTime(HANDLE handle);

	// ��ǰ������Ƶ��ʱ��, ��λ��.
	EXPORT_API double Duration(HANDLE handle);

	// ��ǰ������Ƶ�Ŀ��, ��λ����.
	EXPORT_API void VideoSize(HANDLE handle, int width, int height);

	// ��ǰ�������, ��λ�ٷֱ�.
	EXPORT_API double Buffering(HANDLE handle);

	// ��Ⱦģ��
	EXPORT_API void SetRenderMode(HANDLE handle, RENDER_MODE mode);

	//����
	EXPORT_API void SetScale(HANDLE handle, float factor);

	// ����
	EXPORT_API void SetRotate(HANDLE handle, double xoffset, double yoffset);

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif
