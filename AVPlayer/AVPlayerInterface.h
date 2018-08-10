#pragma once
#include <Windows.h>
#include <string>
#include "../AVPlayerCore/AVPlayerDefine.h"

typedef void(*funcInit)();
typedef void(*funcUnInit)();
typedef HANDLE(*funcCreatePlayer)();
typedef void(*funcDestoryPlayer)(HANDLE handle);
// ��һ��ý���ļ�
typedef bool(*funcOpen)(HANDLE handle, PLAYER_OPTS &opts, bool bSync);
// ��ʼ������Ƶ.
typedef bool(*funcPlay)(HANDLE handle);
// ��ͣ����.
typedef void(*funcPause)(HANDLE handle);
// ��������.
typedef void(*funcResume)(HANDLE handle);
// ֹͣ����.
typedef void(*funcStop)(HANDLE handle);
// ����״̬
typedef PLAY_STATUS (*funcStatus)(HANDLE handle);
// �ȴ�����ֱ�����.
typedef bool(*funcWaitForCompletion)(HANDLE handle);
// �ر�ý��
typedef void(*funcClose)(HANDLE handle);
// seek��ĳ��ʱ�䲥��, ����Ƶʱ���İٷֱ�.
typedef void(*funcSeekTo)(HANDLE handle, double fact);
// ��������������С.
typedef void(*funcVolume)(HANDLE handle, int nVolume);
// ��������.
typedef void(*funcMute)(HANDLE handle, bool s);
// ȫ���л�.
typedef void(*funcFullScreen)(HANDLE handle, bool bfull);
// ���ص�ǰ����ʱ��.
typedef double(*funcCurrentPlayTime)(HANDLE handle);
// ��ǰ������Ƶ��ʱ��, ��λ��.
typedef double(*funcDuration)(HANDLE handle);
// ��ǰ������Ƶ�Ŀ��, ��λ����.
typedef void(*funcVideoSize)(HANDLE handle, int width, int height);
// ��ǰ�������, ��λ�ٷֱ�.
typedef double(*funcBuffering)(HANDLE handle);
