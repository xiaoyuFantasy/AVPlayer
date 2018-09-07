#pragma once
#include "RenderDefine.h"

#define PANO_APICALL					__declspec(dllexport)
#define PANO_APIENTRY					__stdcall

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

#include "CSharpInterface.h"
#pragma comment(lib, "IVRRender.lib")

class CGlRender : public IRender
{
public:
	CGlRender();
	virtual ~CGlRender();

public:
	bool InitRender() override;
	bool CreateRender(HWND hWnd, int nWidth, int nHeight, int pixelFormat = AV_PIX_FMT_YUV420P) override;
	void DestoryRender() override;
	bool SetRenderMode(RENDER_MODE) override;
	void SetRenderSize(int width, int height) override;
	void RenderFrameData(FramePtr pFrame) override;
	void SetScale(float factor) override;
	void SetRotate(float x, float y) override;
	void SetScroll(float latitude, float longitude) override;
	void SetReverse(bool filp) override;

protected:
	void InitGL();

private:
	std::atomic_bool m_bClose = false;
	HWND		m_hWnd = nullptr; // 保存窗口句柄.
	int			m_nVideoWidth;// 视频宽.
	int			m_nVideoHeight;// 视频高.
	bool		m_bKeepAspect;// 是否启用宽高比.
	float		m_fWindowAspect;// 宽高比.
	RECT		m_rcLastClient;// 最后位置参数.
	int			m_nWndWidth;// 当前宽.
	int			m_nWndHeight;// 当前高.
	//转换
	int		m_nPixelFormat = AV_PIX_FMT_YUV420P;
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
	HWND				m_hNativeRender = nullptr;
};

