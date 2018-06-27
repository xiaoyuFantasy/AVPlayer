// ���� ifdef ���Ǵ���ʹ�� DLL �������򵥵�
// ��ı�׼�������� DLL �е������ļ��������������϶���� AVPLAYERCORE_EXPORTS
// ���ű���ġ���ʹ�ô� DLL ��
// �κ�������Ŀ�ϲ�Ӧ����˷��š�������Դ�ļ��а������ļ����κ�������Ŀ���Ὣ
// AVPLAYERCORE_API ������Ϊ�Ǵ� DLL ����ģ����� DLL ���ô˺궨���
// ������Ϊ�Ǳ������ġ�
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
	* ��������	: ��ʼ������������
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API InitPlayer();

	/**
	* ��������	: ����ʼ������������
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API UnInitPlayer();

	/**
	* ��������	: ����������
	* ����ֵ	: ���������
	*/
	HANDLE  PLAYERCORE_API CreateMPlayer();

	/**
	* ��������	: ���ٲ�����
	* ������ ���������
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API DestoryMPlayer(HANDLE handle);

	/**
	* ��������	: ����
	* ����ֵ	: ��
	*/
	//void PLAYERCORE_API SetOptions(HANDLE handle, PLAYER_OPTIONS &opts);

	/**
	* ��������	: ��������
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API SetVolume(HANDLE handle, int nVolume);

	/**
	* ��������	: ��ȡ����
	* ����ֵ	: ��
	*/
	int PLAYERCORE_API GetVolume(HANDLE handle);

	/**
	* ��������	: ����
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API SetMuted(HANDLE handle, bool bMuted);

	/**
	* ��������	: ����
	* ����ֵ	: ��
	*/
	bool PLAYERCORE_API IsMuted(HANDLE handle);

	/**
	* ��������	: ȫ��ģʽ
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API SetPanoType(HANDLE handle, PLAY_MODE type);

	/**
	* ��������	: ȫ������
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API SetPanoScale(HANDLE handle, float factor);

	/**
	* ��������	: ȫ����ת
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API SetPanoRotate(HANDLE handle, float x, float y);

	/**
	* ��������	: ȫ��С������ת
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API SetPanoScroll(HANDLE handle, float latitude, float longitude);

	/**
	* ��������	: ����
	* ����ֵ	: �ɹ� or ʧ��
	*/
	bool PLAYERCORE_API Play(HANDLE handle, PLAYER_OPTS & opts);

	/**
	* ��������	: ֹͣ����
	* ����ֵ	: �ɹ� or ʧ��
	*/
	bool PLAYERCORE_API Stop(HANDLE handle);

	/**
	* ��������	: �Ƿ����ڲ���
	* ����ֵ	: �Ƿ�
	*/
	bool PLAYERCORE_API IsPlaying(HANDLE handle);

	/**
	* ��������	: ��ͣ����
	* ����ֵ	: �Ƿ�
	*/
	void PLAYERCORE_API Pause(HANDLE handle, bool bPause = true);

	/**
	* ��������	: �Ƿ���ͣ����
	* ����ֵ	: �Ƿ�
	*/
	bool PLAYERCORE_API IsPaused(HANDLE handle);

	/**
	* ��������	: ��ת
	* ����ֵ	: �Ƿ�
	*/
	void PLAYERCORE_API Seek(HANDLE handle, int nPos);

	/**
	* ��������	: ���ò�������С
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API SetWindowSize(HANDLE handle, int nWidth, int nHeight);

	/**
	* ��������	: �ر�Console
	* ����ֵ	: ��
	*/
	void PLAYERCORE_API OpenConsole(bool bOpen = true);

#ifdef __cplusplus
}  /* end of the 'extern "C"' block */
#endif


