#include "stdafx.h"
#include "VideoWnd.h"


CVideoWnd::CVideoWnd()
{
}


CVideoWnd::~CVideoWnd()
{
}

LPCTSTR CVideoWnd::GetWindowClassName() const
{
	return L"VideoWnd";
}

CDuiString CVideoWnd::GetSkinFolder()
{
	return L"";
}

CDuiString CVideoWnd::GetSkinFile()
{
	return L"video_frame.xml";
}

void CVideoWnd::OnFinalMessage(HWND hWnd)
{
	delete this;
}

void CVideoWnd::InitWindow()
{
	m_hPlayerModule = LoadLibrary(L"AVPlayerCore.dll");
	if (m_hPlayerModule)
	{
		m_funcInit = (funcInit)GetProcAddress(m_hPlayerModule, "InitPlayer");
		m_funcUnInit = (funcUnInit)GetProcAddress(m_hPlayerModule, "UnInitPlayer");

		m_funcCreatePlayer = (funcCreatePlayer)GetProcAddress(m_hPlayerModule, "CreateMPlayer");
		m_funcDesotryPlayer = (funcDestoryPlayer)GetProcAddress(m_hPlayerModule, "DestoryMPlayer");
		m_funcSetOptions = (funcSetOptions)(GetProcAddress)(m_hPlayerModule, "SetOptions");
		m_funcPlay = (funcPlay)GetProcAddress(m_hPlayerModule, "Play");
		m_funcStop = (funcStop)GetProcAddress(m_hPlayerModule, "Stop");
		m_funcIsPlaying = (funcIsPlaying)GetProcAddress(m_hPlayerModule, "IsPlaying");
		m_funcPause = (funcPause)GetProcAddress(m_hPlayerModule, "Pause");
		m_funcIsPaused = (funcIsPaused)GetProcAddress(m_hPlayerModule, "IsPaused");
		m_funcSeek = (funcSeek)GetProcAddress(m_hPlayerModule, "Seek");
		m_funcSetWindowSize = (funcSetWindowSize)GetProcAddress(m_hPlayerModule, "SetWindowSize");
		m_funcSetPanoScale = (funcSetPanoScale)GetProcAddress(m_hPlayerModule, "SetPanoScale");
		m_funcSetPanoRotate = (funcSetPanoRotate)GetProcAddress(m_hPlayerModule, "SetPanoRotate");
		m_funcSetPanoScroll = (funcSetPanoScroll)GetProcAddress(m_hPlayerModule, "SetPanoScroll");
		m_funcSetVolume = (funcSetVolume)GetProcAddress(m_hPlayerModule, "SetVolume");
		m_funcSetMuted = (funcSetMuted)GetProcAddress(m_hPlayerModule, "SetMuted");
		m_funcInit();
		m_hPlayer = m_funcCreatePlayer();
	}
}

void CVideoWnd::Notify(TNotifyUI & msg)
{
	__super::Notify(msg);
}

LRESULT CVideoWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	ResizeVideo();
	return __super::OnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT CVideoWnd::OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	ResizeVideo();
	return __super::OnSizing(uMsg, wParam, lParam, bHandled);
}

LRESULT CVideoWnd::OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	ResizeVideo();
	return __super::OnMove(uMsg, wParam, lParam, bHandled);
}

LRESULT CVideoWnd::OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	ResizeVideo();
	return __super::OnMoving(uMsg, wParam, lParam, bHandled);
}

void CVideoWnd::ResizeVideo()
{
	CDuiRect rc;
	GetWindowRect(m_hWnd, &rc);
	if (m_hPlayer)
		m_funcSetWindowSize(m_hPlayer, rc.GetWidth(), rc.GetHeight());
}

