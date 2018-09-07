//========================================================
/**
*  @file  CSharpInterface.h
*
*  ��Ŀ����	: ����VR Windows��
*  �ļ�����	: ����C#���ýӿں���
*  ����ƽ̨	: Windows7/10
*
*  ����		: LAI ZHONG AN
*  �����ʼ�	: zhonganlai@gmail.com
*  ��������	: 20017-08-17
*  �޸�����	: 20017-08-21
*
*/
//========================================================
#ifndef __CSHARPINTERFACE_H__
#define __CSHARPINTERFACE_H__
#include "stdafx.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * ��������	: �������ڷ���
 * ����1	: �����ھ��
 * ����2	: ��ǰʵ�����
 * ����3	: ���Σ����п��� nullptr��
 * ����4	: ������Ϣ�ṹ��
 * ����ֵ	: �ɹ����ش������ھ����ʧ�ܷ���0
 */
PANO_APICALL HWND PANO_APIENTRY NativeOnCreate(HWND hwndParent, HINSTANCE hInstance, LPVOID lpParam, PANO_INFO* panoramaInfo);

PANO_APICALL void PANO_APIENTRY NativeOnResume(HWND hwnd);

PANO_APICALL void PANO_APIENTRY NativeOnPause(HWND hwnd);

/**
 * ��������	: ɾ��ָ������
 * ����1	: ���ھ��
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeOnDestroy(HWND hwnd);

/**
 * ��������	: ���õ�ǰ���ڲ���ģʽ
 * ����1	: ���ھ��
 * ����2	: ģʽѡ��
 * ����ֵ	: �ɹ�����true��ʧ�ܷ���false
 */
PANO_APICALL bool PANO_APIENTRY NativeSetPlayMode(HWND hwnd, PLAY_MODE playMode);

/**
 * ��������	: ������Ⱦ����
 * ����1	: ���ھ��
 * ����2	: ��Ⱦ���ݸ�ʽ(YUV420)����д0����
 * ����3	: ��Ⱦ����(��ȾͼƬʱframeDataΪ��NULL����)
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeSetFrameData(HWND hwnd, STREAM_FORMAT format, unsigned char* frameData[]);

/**
 * ��������	: ��Ⱦһ֡����
 * ����1	: ���ھ��
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeDraw(HWND hwnd);

/**
 * ��������	: �����ӿ�
 * ����1	: ���ھ��
 * ����2	: �������Ͻ�X����
 * ����3	: �������Ͻ�Y����
 * ����4	: ��ڿ�
 * ����5	: ���ڸ�
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeUpdateViewport(HWND hwnd, int windowPosX, int windowPosY, int windowWidth, int windowHeight);

/**
 * ��������	: ��ʾ����
 * ����1	: ���ھ��
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeShowWindows(HWND hwnd);

/**
 * ��������	: ���ش���
 * ����1	: ���ھ��
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeHiddenWindows(HWND hwnd);

/**
 * ��������	: ��������
 * ����1	: ���ھ��
 * ����2	: ��������
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeSetScale(HWND hwnd, float factor);

/**
 * ��������	: ������ת
 * ����1	: ���ھ��
 * ����2	: x����ֵ
 * ����3	: y����ֵ
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeSetRotate(HWND hwnd, float x, float y);

/**
 * ��������	: ����С������ת
 * ����1	: ���ھ��
 * ����2	: γ��ֵ��xֵ -�� ~ �У�
 * ����3	: ����ֵ��yֵ -�� ~ �У�
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeSetScroll(HWND hwnd, float latitude, float longitude);

/**
 * ��������	: ��ת��ʾ
 * ����1	: ���ھ��
 * ����2	: �Ƿ�ת
 * ����ֵ	: ��
 */
PANO_APICALL void PANO_APIENTRY NativeSetReverse(HWND hwnd, bool filp);

#ifdef __cplusplus
}
#endif

#endif	// __CSHARPINTERFACE_H__