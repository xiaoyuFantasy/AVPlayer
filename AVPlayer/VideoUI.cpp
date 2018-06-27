#include "stdafx.h"
#include "VideoUI.h"


CVideoUI::CVideoUI()
{
}


CVideoUI::~CVideoUI()
{
}

void CVideoUI::DoInit()
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
		m_funcSetWindowSize = (funcSetWindowSize)GetProcAddress(m_hPlayerModule, "SetWindowSize");

		m_funcSetPanoScale = (funcSetPanoScale)GetProcAddress(m_hPlayerModule, "SetPanoScale");
		m_funcSetPanoRotate = (funcSetPanoRotate)GetProcAddress(m_hPlayerModule, "SetPanoRotate");
		m_funcSetPanoScroll = (funcSetPanoScroll)GetProcAddress(m_hPlayerModule, "SetPanoScroll");

		m_funcInit();
		m_hPlayer = m_funcCreatePlayer();
	}
}

void CVideoUI::SetVisible(bool bVisible)
{

}
