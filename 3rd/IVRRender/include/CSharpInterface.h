//========================================================
/**
*  @file  CSharpInterface.h
*
*  项目描述	: 万视VR Windows版
*  文件描述	: 程序C#调用接口函数
*  适用平台	: Windows7/10
*
*  作者		: LAI ZHONG AN
*  电子邮件	: zhonganlai@gmail.com
*  创建日期	: 20017-08-17
*  修改日期	: 20017-08-21
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
 * 功能描述	: 创建窗口方法
 * 参数1	: 父窗口句柄
 * 参数2	: 当前实例句柄
 * 参数3	: 传参（可有可无 nullptr）
 * 参数4	: 窗口信息结构体
 * 返回值	: 成功返回创建窗口句柄，失败返回0
 */
PANO_APICALL HWND PANO_APIENTRY NativeOnCreate(HWND hwndParent, HINSTANCE hInstance, LPVOID lpParam, PANO_INFO* panoramaInfo);

PANO_APICALL void PANO_APIENTRY NativeOnResume(HWND hwnd);

PANO_APICALL void PANO_APIENTRY NativeOnPause(HWND hwnd);

/**
 * 功能描述	: 删除指定窗口
 * 参数1	: 窗口句柄
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeOnDestroy(HWND hwnd);

/**
 * 功能描述	: 设置当前窗口播放模式
 * 参数1	: 窗口句柄
 * 参数2	: 模式选择
 * 返回值	: 成功翻译true，失败返回false
 */
PANO_APICALL bool PANO_APIENTRY NativeSetPlayMode(HWND hwnd, PLAY_MODE playMode);

/**
 * 功能描述	: 设置渲染数据
 * 参数1	: 窗口句柄
 * 参数2	: 渲染数据格式(YUV420)，填写0即可
 * 参数3	: 渲染数据(渲染图片时frameData为空NULL即可)
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeSetFrameData(HWND hwnd, STREAM_FORMAT format, unsigned char* frameData[]);

/**
 * 功能描述	: 渲染一帧数据
 * 参数1	: 窗口句柄
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeDraw(HWND hwnd);

/**
 * 功能描述	: 更新视口
 * 参数1	: 窗口句柄
 * 参数2	: 窗口左上角X坐标
 * 参数3	: 窗口左上角Y坐标
 * 参数4	: 宽口宽
 * 参数5	: 窗口高
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeUpdateViewport(HWND hwnd, int windowPosX, int windowPosY, int windowWidth, int windowHeight);

/**
 * 功能描述	: 显示窗口
 * 参数1	: 窗口句柄
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeShowWindows(HWND hwnd);

/**
 * 功能描述	: 隐藏窗口
 * 参数1	: 窗口句柄
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeHiddenWindows(HWND hwnd);

/**
 * 功能描述	: 设置缩放
 * 参数1	: 窗口句柄
 * 参数2	: 缩放因子
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeSetScale(HWND hwnd, float factor);

/**
 * 功能描述	: 设置旋转
 * 参数1	: 窗口句柄
 * 参数2	: x弧度值
 * 参数3	: y弧度值
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeSetRotate(HWND hwnd, float x, float y);

/**
 * 功能描述	: 鱼眼小行星旋转
 * 参数1	: 窗口句柄
 * 参数2	: 纬度值（x值 -π ~ π）
 * 参数3	: 经度值（y值 -π ~ π）
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeSetScroll(HWND hwnd, float latitude, float longitude);

/**
 * 功能描述	: 反转显示
 * 参数1	: 窗口句柄
 * 参数2	: 是否反转
 * 返回值	: 无
 */
PANO_APICALL void PANO_APIENTRY NativeSetReverse(HWND hwnd, bool filp);

#ifdef __cplusplus
}
#endif

#endif	// __CSHARPINTERFACE_H__