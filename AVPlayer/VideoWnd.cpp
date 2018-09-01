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

		m_funcCreatePlayer = (funcCreatePlayer)GetProcAddress(m_hPlayerModule, "CreateAVPlayer");
		m_funcDesotryPlayer = (funcDestoryPlayer)GetProcAddress(m_hPlayerModule, "DestoryAVPlayer");
		m_funcOpen = (funcOpen)GetProcAddress(m_hPlayerModule, "Open");
		m_funcPlay = (funcPlay)GetProcAddress(m_hPlayerModule, "Play");
		m_funcPause = (funcPause)GetProcAddress(m_hPlayerModule, "Pause");
		m_funcResume = (funcResume)GetProcAddress(m_hPlayerModule, "Resume");
		m_funcStop = (funcStop)GetProcAddress(m_hPlayerModule, "Stop");
		m_funcStatus = (funcStatus)GetProcAddress(m_hPlayerModule, "Status");
		m_funcWaitForCompletion = (funcWaitForCompletion)GetProcAddress(m_hPlayerModule, "WaitForCompletion");
		m_funcClose = (funcClose)GetProcAddress(m_hPlayerModule, "Close");
		m_funcSeekTo = (funcSeekTo)GetProcAddress(m_hPlayerModule, "SeekTo");
		m_funcVolume = (funcVolume)GetProcAddress(m_hPlayerModule, "Volume");
		m_funcMute = (funcMute)GetProcAddress(m_hPlayerModule, "Mute");
		m_funcFullScreen = (funcFullScreen)GetProcAddress(m_hPlayerModule, "FullScreen");
		m_funcCurrentPlayTime = (funcCurrentPlayTime)GetProcAddress(m_hPlayerModule, "CurrentPlayTime");
		m_funcDuration = (funcDuration)GetProcAddress(m_hPlayerModule, "Duration");
		m_funcVideoSize = (funcVideoSize)GetProcAddress(m_hPlayerModule, "VideoSize");
		m_funcBuffering = (funcBuffering)GetProcAddress(m_hPlayerModule, "Buffering");
		m_funcInit();
		m_hPlayer = m_funcCreatePlayer();
	}
	else
	{
		int err = GetLastError();
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
		m_funcVideoSize(m_hPlayer, rc.GetWidth(), rc.GetHeight());
}

