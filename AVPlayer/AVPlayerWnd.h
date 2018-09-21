#pragma once
#include "AVPlayerInterface.h"
#include "VideoWnd.h"
#include "UIWnd.h"
#include "SettingDialog.h"

class CAVPlayerWnd : public WindowImplBase
{
public:
	CAVPlayerWnd();
	virtual ~CAVPlayerWnd();

public:
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFolder();
	CDuiString GetSkinFile();
	CControlUI* CreateControl(LPCTSTR pstrClass);
	void OnFinalMessage(HWND hWnd);
	void InitWindow();
	void Notify(TNotifyUI &msg);
	void SetCmdLine(std::map<std::wstring, std::wstring> &mapCmd);
	LRESULT HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
	int ParseCmdLine(const wchar_t* lpCmdLine, std::map<std::wstring, std::wstring>& pMapCmdLine);
	static void FuncPlayerEvent(void* user_data, const PLAYER_EVENT e, LPVOID pData);
	static void DurationCallback(void *userdata, int64_t duration);
	static void ProgressCallback(void *userdata, int64_t progress);
	static void VideoCallback(void *userdata, int nWidth, int nHeight);
	static void ErrorCallback(void *userdata, int err, std::string strMsg);

private:
	PLAYER_OPTS m_opts;
	std::shared_ptr<CSettingDialog> m_pSettingDlg;
	double		m_proportion = 0.0;
	CLabelUI*	m_pLabelName = nullptr;
	CVideoWnd*	m_pVideo = nullptr;
	CWndUI*		m_pVideoLayout = nullptr;
	CSliderUI*	m_pSliderPlay = nullptr;
	CButtonUI*	m_pBtnStop = nullptr;
	CButtonUI*	m_pBtnPrevious = nullptr;
	CButtonUI*	m_pBtnPlay = nullptr;
	CButtonUI*	m_pBtnPause = nullptr;
	CButtonUI*	m_pBtnNext = nullptr;
	CButtonUI*	m_pBtnVolume = nullptr;
	CButtonUI*	m_pBtnVolumeZero = nullptr;
	CSliderUI*	m_pSliderVolume = nullptr;
	float		m_scale = 0.5f;
	std::map<std::wstring, std::wstring>	m_mapCmd;
	std::mutex	m_mutexCallback;
};

