#pragma once
#include "RenderDefine.h"

#define PANO_APICALL					__declspec(dllexport)
#define PANO_APIENTRY					__stdcall

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
	HWND		m_hWnd = nullptr; // ���洰�ھ��.
	int			m_nVideoWidth;// ��Ƶ��.
	int			m_nVideoHeight;// ��Ƶ��.
	bool		m_bKeepAspect;// �Ƿ����ÿ�߱�.
	float		m_fWindowAspect;// ��߱�.
	RECT		m_rcLastClient;// ���λ�ò���.
	int			m_nWndWidth;// ��ǰ��.
	int			m_nWndHeight;// ��ǰ��.
	//ת��
	int		m_nPixelFormat = AV_PIX_FMT_YUV420P;
	SwsContext*			m_pSwsCtx = nullptr;
	uint8_t*			m_pVideoBuffer = nullptr;
	AVFrame*			m_pFrameOut = nullptr;
	HWND				m_hNativeRender = nullptr;
};

