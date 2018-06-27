#pragma once
#include "AVPlayerInterface.h"

class CVideoUI :
	public CControlUI
{
public:
	CVideoUI();
	virtual ~CVideoUI();

public:
	virtual void DoInit();

	virtual void SetVisible(bool bVisible = true);

private:
	HMODULE			m_hPlayerModule = NULL;
	HWND			m_hPlayerWnd = nullptr;
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
	funcSetWindowSize	m_funcSetWindowSize = nullptr;
	HANDLE				m_hPlayer = nullptr;
};

