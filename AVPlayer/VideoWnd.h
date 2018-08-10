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
	funcOpen			m_funcOpen = nullptr;
	funcPlay			m_funcPlay = nullptr;
	funcPause			m_funcPause = nullptr;
	funcResume			m_funcResume = nullptr;
	funcStop			m_funcStop = nullptr;
	funcStatus			m_funcStatus = nullptr;
	funcWaitForCompletion	m_funcWaitForCompletion = nullptr;
	funcClose			m_funcClose = nullptr;
	funcSeekTo			m_funcSeekTo = nullptr;
	funcVolume			m_funcVolume = nullptr;
	funcMute			m_funcMute = nullptr;
	funcFullScreen		m_funcFullScreen = nullptr;
	funcCurrentPlayTime m_funcCurrentPlayTime = nullptr;
	funcDuration		m_funcDuration = nullptr;
	funcVideoSize		m_funcVideoSize = nullptr;
	funcBuffering		m_funcBuffering = nullptr;
	HANDLE				m_hPlayer = nullptr;
};

