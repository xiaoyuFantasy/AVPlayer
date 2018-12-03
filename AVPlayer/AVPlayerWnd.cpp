#include "stdafx.h"
#include "AVPlayerWnd.h"
#include <atlconv.h>
#include <Shellapi.h>
#pragma comment(lib, "Shell32.lib")

std::wstring StringToWString(const std::string &str)
{
	std::wstring wstr(str.length(), L' ');
	std::copy(str.begin(), str.end(), wstr.begin());
	return wstr;
}

//只拷贝低字节至string中
std::string WStringToString(const std::wstring &wstr)
{
	std::string str(wstr.length(), ' ');
	std::copy(wstr.begin(), wstr.end(), str.begin());
	return str;
}

CAVPlayerWnd::CAVPlayerWnd()
{
}


CAVPlayerWnd::~CAVPlayerWnd()
{
}

LPCTSTR CAVPlayerWnd::GetWindowClassName() const
{
	return L"AVPlayerWnd";
}

CDuiString CAVPlayerWnd::GetSkinFolder()
{
	return L"";
}

CDuiString CAVPlayerWnd::GetSkinFile()
{
	return L"player_frame.xml";
}

CControlUI * CAVPlayerWnd::CreateControl(LPCTSTR pstrClass)
{
	if (_tcscmp(pstrClass, _T("PlayerWindow")) == 0)
	{
		CWndUI *pUI = new CWndUI;
		m_pVideo = new CVideoWnd;
		m_pVideo->SetCmdLine(m_mapCmd);
		m_pVideo->Create(m_hWnd, L"VidaoPanel", UI_WNDSTYLE_CHILD, NULL);
		pUI->Attach(m_pVideo->GetHWND());
		return pUI;
	}

	return nullptr;
}

void CAVPlayerWnd::OnFinalMessage(HWND hWnd)
{
	PostQuitMessage(IDOK);
}

void CAVPlayerWnd::InitWindow()
{
	m_pVideoLayout = (CWndUI*)m_PaintManager.FindControl(L"video_player");
	m_pLabelName = (CLabelUI*)m_PaintManager.FindControl(L"video_name");
	m_pSliderPlay = (CSliderUI*)m_PaintManager.FindControl(L"sliderPlay");
	m_pBtnStop = (CButtonUI*)m_PaintManager.FindControl(L"btnStop");
	m_pBtnPrevious = (CButtonUI*)m_PaintManager.FindControl(L"btnPrevious");
	m_pBtnPlay = (CButtonUI*)m_PaintManager.FindControl(L"btnPlay");
	m_pBtnPause = (CButtonUI*)m_PaintManager.FindControl(L"btnPause");
	m_pBtnNext = (CButtonUI*)m_PaintManager.FindControl(L"btnNext");
	m_pBtnVolume = (CButtonUI*)m_PaintManager.FindControl(L"btnVolume");
	m_pBtnVolumeZero = (CButtonUI*)m_PaintManager.FindControl(L"btnVolumeZero");
	m_pSliderVolume = (CSliderUI*)m_PaintManager.FindControl(L"sliderVol");

	if (::IsWindow(m_pVideo->GetHWND()))
	{
		m_opts.hWnd = m_pVideo->GetHWND();
		m_opts.user_data = this;
		m_opts.hInstance = m_PaintManager.GetInstance();
		m_opts.funcEvent = CAVPlayerWnd::FuncPlayerEvent;
	}
}

void CAVPlayerWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_CLICK)
	{
		if (_tcscmp(msg.pSender->GetName(), _T("btnPlay")) == 0)
		{
			if (m_pSettingDlg && ::IsWindow(m_pSettingDlg->GetHWND()))
			{
				SetForegroundWindow(m_pSettingDlg->GetHWND());
			}
			else
			{
				m_pSettingDlg.reset();
				m_pSettingDlg = std::make_shared<CSettingDialog>();
				m_pSettingDlg->SetCallback([&](std::wstring wstrPath, int video_type, int decode_type) {
					wchar_t szUrl[MAX_PATH + 1] = { 0 };
					_tcscpy_s(szUrl, MAX_PATH, wstrPath.c_str());
					::PathStripPath(szUrl);
					m_pLabelName->SetText(szUrl);
					m_opts.video_type = (AV_VIDEO_TYPE)video_type;// VIDEO_TYPE::NORMAL_TYPE;
					m_opts.decode_type = (AV_DECODE_TYPE)decode_type;
					m_opts.strPath = CW2A(wstrPath.c_str(), CP_UTF8);
					m_pVideo->SetOptions(m_opts);
					m_pVideo->m_funcOpen(m_pVideo->m_hPlayer, m_opts, true);
					m_pBtnPlay->SetVisible(false);
					m_pBtnPause->SetVisible();
					m_pBtnStop->SetEnabled();
					m_pVideoLayout->SetVisible();
				});
				m_pSettingDlg->Create(m_hWnd, L"SettingDialog", UI_WNDSTYLE_DIALOG, UI_WNDSTYLE_EX_DIALOG);
				m_pSettingDlg->CenterWindow();
				m_pSettingDlg->ShowWindow();
			}			
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnRenderMode")) == 0)
		{
			m_pVideo->m_funcSetRenderMode(m_pVideo->m_hPlayer, AV_RENDER_MODE::STANDARD);
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnStop")) == 0)
		{
			//if (m_pVideo->m_funcIsPlaying(m_pVideo->m_hPlayer))
			{
				m_pVideo->m_funcStop(m_pVideo->m_hPlayer);
				m_pBtnPlay->SetVisible();
				m_pBtnPause->SetVisible(false);
				m_pBtnStop->SetEnabled(false);
				m_pVideoLayout->SetVisible(false);
			}
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnPrevious")) == 0)
		{
			/*int value = m_pSliderPlay->GetValue();
			m_pVideo->m_funcSeek(m_pVideo->m_hPlayer, value - 5);*/
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnNext")) == 0)
		{
			/*int value = m_pSliderPlay->GetValue();
			m_pVideo->m_funcSeek(m_pVideo->m_hPlayer, value + 5);*/
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnVolume")) == 0)
		{
			m_pBtnVolume->SetVisible(false);
			m_pBtnVolumeZero->SetVisible();
			m_pVideo->m_funcMute(m_pVideo->m_hPlayer, true);
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("btnVolumeZero")) == 0)
		{
			m_pBtnVolume->SetVisible();
			m_pBtnVolumeZero->SetVisible(false);
			m_pVideo->m_funcVolume(m_pVideo->m_hPlayer, false);
		}
	}
	else if (msg.sType == DUI_MSGTYPE_VALUECHANGED)
	{
		if (_tcscmp(msg.pSender->GetName(), _T("sliderPlay")) == 0)
		{
			/*int value = m_pSliderPlay->GetValue();
			m_pVideo->m_funcSeek(m_pVideo->m_hPlayer, value);*/
		}
		else if (_tcscmp(msg.pSender->GetName(), _T("sliderVol")) == 0)
		{
			int value = m_pSliderVolume->GetValue();
			m_pVideo->m_funcVolume(m_pVideo->m_hPlayer, value);
		}
	}
	__super::Notify(msg);
}

void CAVPlayerWnd::SetCmdLine(std::map<std::wstring, std::wstring>& mapCmd)
{
	m_mapCmd = mapCmd;
}

LRESULT CAVPlayerWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (uMsg == PLAYER_MSG_DURATION)
	{
		m_pSliderPlay->SetMinValue(0);
		m_pSliderPlay->SetMaxValue(wParam);
		m_pSliderPlay->SetEnabled();
	}
	else if (uMsg == PLAYER_MSG_PROGRESS)
		m_pSliderPlay->SetValue(wParam);
	else if (uMsg == PLAYER_MSG_PLAY)
		m_pVideo->m_funcPlay(m_pVideo->m_hPlayer);
	
	return __super::HandleCustomMessage(uMsg, wParam, lParam, bHandled);
}

int CAVPlayerWnd::ParseCmdLine(const wchar_t * lpCmdLine, std::map<std::wstring, std::wstring>& pMapCmdLine)
{
	int nArgs = 0;
	LPWSTR * szArglist = CommandLineToArgvW(lpCmdLine, &nArgs);

	for (int i = 0; i < nArgs; i++)
	{
		if (wcsncmp(L"//", szArglist[i], 1) != 0)
		{
			continue;
		}
		if (i + 1 < nArgs) //结束  
		{
			if (wcsncmp(L"//", szArglist[i + 1], 1) != 0)
			{
				pMapCmdLine.insert(std::make_pair(szArglist[i], szArglist[i + 1]));
				i++;
				continue;
			}
		}
		pMapCmdLine.insert(std::make_pair(szArglist[i], L"1"));
	}
	LocalFree(szArglist);
	return 0;
}

void CAVPlayerWnd::FuncPlayerEvent(void* user_data, const PLAYER_EVENT e, LPVOID pData)
{
	CAVPlayerWnd * pAvPlayer = (CAVPlayerWnd*)user_data;
	pAvPlayer->m_mutexCallback.lock();
	if (PlayerOpening == e)
		::PostMessage(pAvPlayer->GetHWND(), PLAYER_MSG_PLAY, (WPARAM)0, (LPARAM)0);
	else if (PlayerError == e)
	{
		PlayerErrorSt* pSt = (PlayerErrorSt*)pData;
		pAvPlayer->m_pVideo->m_pLabelMsg->SetText(CA2W(pSt->strErrMsg.c_str(), CP_UTF8));
	}
	pAvPlayer->m_mutexCallback.unlock();
}

void CAVPlayerWnd::DurationCallback(void * userdata, int64_t duration)
{
	CAVPlayerWnd *player = (CAVPlayerWnd*)userdata;
	::PostMessage(player->GetHWND(), PLAYER_MSG_DURATION, duration, NULL);
}

void CAVPlayerWnd::ProgressCallback(void * userdata, int64_t progress)
{
	CAVPlayerWnd *player = (CAVPlayerWnd*)userdata;
	::PostMessage(player->GetHWND(), PLAYER_MSG_PROGRESS, progress, NULL);
}

void CAVPlayerWnd::VideoCallback(void * userdata, int nWidth, int nHeight)
{
	CAVPlayerWnd *player = (CAVPlayerWnd*)userdata;
	player->m_proportion = (double)nWidth / (double)nHeight;
	player->m_pVideoLayout->SetProportion(player->m_proportion);
}

void CAVPlayerWnd::ErrorCallback(void * userdata, int err, std::string strMsg)
{
}



