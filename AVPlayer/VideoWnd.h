#pragma once
#include "AVPlayerInterface.h"

class CVideoWnd : public WindowImplBase
{
public:
	CVideoWnd();
	virtual ~CVideoWnd();

public:
	LPCTSTR GetWindowClassName() const;
	CDuiString GetSkinFolder();
	CDuiString GetSkinFile();
	void OnFinalMessage(HWND hWnd);
	void InitWindow();
	void Notify(TNotifyUI &msg);

protected:
	virtual LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMove(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	virtual LRESULT OnMoving(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	void ResizeVideo();

public:
	HMODULE			m_hPlayerModule = NULL;
	funcInit		m_funcInit = nullptr;
	funcUnInit		m_funcUnInit = nullptr;
	funcCreatePlayer	m_funcCreatePlayer = nullptr;
	funcDestoryPlayer	m_funcDesotryPlayer = nullptr;
	funcSetOptions		m_funcSetOptions = nullptr;
	funcSetPanoScale    m_funcSetPanoScale = nullptr;
	funcSetPanoRotate	m_funcSetPanoRotate = nullptr;
	funcSetPanoScroll	m_funcSetPanoScroll = nullptr;
	funcPlay			m_funcPlay = nullptr;
	funcStop			m_funcStop = nullptr;
	funcIsPlaying		m_funcIsPlaying = nullptr;
	funcPause			m_funcPause = nullptr;
	funcIsPaused		m_funcIsPaused = nullptr;
	funcSeek			m_funcSeek = nullptr;
	funcSetWindowSize	m_funcSetWindowSize = nullptr;
	funcSetVolume		m_funcSetVolume = nullptr;
	funcSetMuted		m_funcSetMuted = nullptr;
	HANDLE				m_hPlayer = nullptr;
};

