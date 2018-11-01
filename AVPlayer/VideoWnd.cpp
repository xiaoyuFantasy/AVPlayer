#include "stdafx.h"
#include "VideoWnd.h"
#include "ipc/ipc_message.h"
#include "misc\AES_Encryptor.h"
#include <Wtsapi32.h>
#pragma comment(lib, "Wtsapi32.lib")

#define IPC_TIMEOUT (WM_USER + 1)

std::vector<std::string> CommandLineToArgs(std::string str, std::string pattern = " ")
{
	std::string::size_type pos;
	std::vector<std::string> result;
	str += pattern;//扩展字符串以方便操作  
	int size = str.size();
	for (int i = 0; i<size; i++)
	{
		pos = str.find(pattern, i);
		if (pos<size)
		{
			std::string s = str.substr(i, pos - i);
			result.push_back(s);
			i = pos + pattern.size() - 1;
		}
	}
	return result;
}

int ParseCmdLine(const char * lpCmdLine, std::map<std::string, std::string>& pMapCmdLine)
{
	std::vector<std::string> vectArgs = CommandLineToArgs(lpCmdLine);
	int nArgs = vectArgs.size();
	for (int i = 0; i < nArgs; i++)
	{
		if (strncmp("//", vectArgs[i].c_str(), 1) != 0)
			continue;

		if (i + 1 < nArgs) //结束  
		{
			if (strncmp("//", vectArgs[i + 1].c_str(), 1) != 0)
			{
				pMapCmdLine.insert(std::make_pair(vectArgs[i], vectArgs[i + 1]));
				i++;
				continue;
			}
		}
		pMapCmdLine.insert(std::make_pair(vectArgs[i], "1"));
	}
	return 0;
}

CVideoWnd::CVideoWnd()
{
}


CVideoWnd::~CVideoWnd()
{
}

LPCTSTR CVideoWnd::GetWindowClassName() const
{
	return L"avplayer_wnd";
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
	Send("launch_quit");
	PostQuitMessage(0);
	delete this;
}

void CVideoWnd::InitWindow()
{
	ParseCommandLine();
	InitIPC();
	m_pVideoLayout = (CVerticalLayoutUI*)m_PaintManager.FindControl(_T("video_layout"));
	m_pLabelMsg = (CLabelUI*)m_PaintManager.FindControl(L"msg");
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
		m_funcSetScale = (funcSetScale)GetProcAddress(m_hPlayerModule, "SetScale");
		m_funcSetRotate = (funcSetRotate)GetProcAddress(m_hPlayerModule, "SetRotate");
		m_funcSetRenderMode = (funcSetRenderMode)GetProcAddress(m_hPlayerModule, "SetRenderMode");
		m_funcInit();
		m_hPlayer = m_funcCreatePlayer();
	}
	else
	{
		int err = GetLastError();
		m_pLabelMsg->SetText(L"Load AVPlayerCore.dll Failed!!!");
	}

	::WTSRegisterSessionNotification(m_hWnd, NOTIFY_FOR_ALL_SESSIONS);
}

void CVideoWnd::Notify(TNotifyUI & msg)
{
	if (msg.sType == DUI_MSGTYPE_MENU)
	{
		Send("video_menu");
	}
	else if (msg.sType == DUI_MSGTYPE_DRAG)
	{
		if (_tcscmp(msg.pSender->GetName(), _T("video_layout")) == 0)
		{
			SdkDataObject* dataObj = (SdkDataObject*)msg.wParam;
			string strDeviceJson = "{\"data\":" + to_string(m_nIndex) + ",\"type\":2}";
			char *key = "D464E60E9A2F4FFA";
			AesEncryptor aes((unsigned char*)key);
			string strEncipher = aes.EncryptString(strDeviceJson);

			FORMATETC fmtetc = { 0 };
			fmtetc.dwAspect = DVASPECT_CONTENT;
			fmtetc.lindex = -1;
			fmtetc.cfFormat = CF_TEXT;
			fmtetc.tymed = TYMED_HGLOBAL;
			STGMEDIUM medium = { 0 };
			medium.tymed = TYMED_HGLOBAL;
			medium.hGlobal = GlobalAlloc(GMEM_MOVEABLE, strEncipher.length() + 1);
			char * buff = (char*)GlobalLock(medium.hGlobal);
			strcpy(buff, strEncipher.c_str());
			GlobalUnlock(medium.hGlobal);
			HRESULT hr = dataObj->SetData(&fmtetc, &medium, FALSE);
			return;
		}
	}

	__super::Notify(msg);
}

void CVideoWnd::SetCmdLine(std::map<std::wstring, std::wstring>& mapCmd)
{
	m_mapCmd = mapCmd;
}

void CVideoWnd::Play()
{
	m_opts.hWnd = m_hWnd;
	m_opts.user_data = this;
	m_opts.hInstance = m_PaintManager.GetInstance();
	m_opts.funcEvent = CVideoWnd::FuncPlayerEvent;
	m_opts.video_type = (VIDEO_TYPE)m_nVideoType;
	m_opts.bEnableAudio = false;

	m_opts.strPath = CW2A(m_wstrPlayUrl.c_str(), CP_UTF8);
	::PostMessage(m_hWnd, PLAYER_MSG_OPEN, NULL, NULL);
}

void CVideoWnd::SetOptions(PLAYER_OPTS & opts)
{
	m_opts = opts;
}

void CVideoWnd::Stop()
{
	m_funcStop(m_hPlayer);
}

void CVideoWnd::SetDragEnable(bool bEnable)
{
	if (PANORAMIC_TYPE != m_opts.video_type)
		m_pVideoLayout->SetDragEnabled(true);
}

LRESULT CVideoWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_MOUSEWHEEL)
	{
		if (PANORAMIC_TYPE == m_opts.video_type)
		{
			double fwKeys = GET_KEYSTATE_WPARAM(wParam);
			double zDelta = GET_WHEEL_DELTA_WPARAM(wParam);
			m_funcSetScale(m_hPlayer, (float)zDelta / 120);
		}
	}
	else if (uMsg == WM_LBUTTONDOWN)
	{
		if (PANORAMIC_TYPE == m_opts.video_type)
		{
			m_bLButtonDown = true;
			SetCapture(m_hWnd);
			POINT pt;
			::GetCursorPos(&pt);
			::ClientToScreen(m_hWnd, &pt);
			m_xPos = pt.x;
			m_yPos = pt.y;
		}

		Send("video_lbtndown");
	}
	else if (uMsg == WM_LBUTTONUP)
	{
		if (PANORAMIC_TYPE == m_opts.video_type)
		{
			ReleaseCapture();
			m_bLButtonDown = false;
		}
	}
	else if (uMsg == WM_MOUSEMOVE)
	{
		if (m_bLButtonDown)
		{
			POINT pt;
			::GetCursorPos(&pt);
			::ClientToScreen(m_hWnd, &pt);
			float xoffset = pt.x - m_xPos;
			float yoffset = m_yPos - pt.y;
			m_xPos = pt.x;
			m_yPos = pt.y;
			m_funcSetRotate(m_hPlayer, xoffset, yoffset);
		}
	}
	else if (uMsg == WM_TIMER)
		Close();
	else if (uMsg == WM_LBUTTONDBLCLK)
		Send("video_dbclick");
	else if (uMsg == WM_MOUSEHOVER)
		Send("video_mousehover");
	else if (uMsg == WM_MOUSELEAVE)
		Send("video_mouseleave");
	else if (WM_WTSSESSION_CHANGE == uMsg)
	{
		if (WTS_SESSION_LOCK == wParam)
		{
			if (m_funcStatus(m_hPlayer) != NoneStatus)
			{
				m_bLockScreenPlayedPre = true;
				m_funcClose(m_hPlayer);
			}
			else
				m_bLockScreenPlayedPre = false;
		}
		if (WTS_SESSION_UNLOCK == wParam && m_bLockScreenPlayedPre)
			::PostMessage(m_hWnd, PLAYER_MSG_OPEN, NULL, NULL);
	}
		
	return __super::HandleMessage(uMsg, wParam, lParam);
}

LRESULT CVideoWnd::HandleCustomMessage(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	if (uMsg == PLAYER_MSG_OPEN)
	{
		if (m_funcStatus(m_hPlayer) != NoneStatus)
			return S_FALSE;

		if (m_funcOpen)
			m_funcOpen(m_hPlayer, m_opts, true);
	}

	if (uMsg == PLAYER_MSG_PLAY)
		m_funcPlay(m_hPlayer);
	else if (uMsg == PLAYER_MSG_CLOSE && !m_bLockScreenPlayedPre)
		Close();

	return S_OK;
}

LRESULT CVideoWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	//ResizeVideo();
	return __super::OnSize(uMsg, wParam, lParam, bHandled);
}

LRESULT CVideoWnd::OnSizing(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL & bHandled)
{
	//ResizeVideo();
	return __super::OnSizing(uMsg, wParam, lParam, bHandled);
}

void CVideoWnd::ResizeVideo()
{
	CDuiRect rc;
	GetWindowRect(m_hWnd, &rc);
	if (m_hPlayer)
		m_funcVideoSize(m_hPlayer, rc.GetWidth(), rc.GetHeight());
}

void CVideoWnd::ParseCommandLine()
{
	auto itor = m_mapCmd.find(L"/is_child_wnd");
	if (itor == m_mapCmd.end())
		return;

	m_bChildWnd = itor->second.compare(L"true") == 0 ? true : false;
	if (!m_bChildWnd)
		return;
	
	itor = m_mapCmd.find(L"/video_type");
	if (itor == m_mapCmd.end())
		goto PARSE_FAILED;
	m_nVideoType = stoi(itor->second);

	itor = m_mapCmd.find(L"/video_index");
	if (itor == m_mapCmd.end())
		goto PARSE_FAILED;
	m_nIndex = stoi(itor->second);

	itor = m_mapCmd.find(L"/play_url");
	if (itor == m_mapCmd.end())
		goto PARSE_FAILED;
	m_wstrPlayUrl = itor->second;

	itor = m_mapCmd.find(L"/channel_name");
	if (itor == m_mapCmd.end())
		goto PARSE_FAILED;
	m_wstrChannelName = itor->second;

	return;
PARSE_FAILED:
	Close();
}

void CVideoWnd::InitIPC()
{
	if (m_bChildWnd)
	{
		std::string strChannelName = CW2A(m_wstrChannelName.c_str(), CP_UTF8);
		m_pEndpoint = std::make_shared<IPC::Endpoint>(strChannelName.c_str(), this);
		::SetTimer(m_hWnd, IPC_TIMEOUT, 5000, nullptr);
	}
}

bool CVideoWnd::OnMessageReceived(IPC::Message * msg)
{
	std::string strCmd;
	IPC::MessageReader reader(msg);
	reader.ReadString(&strCmd);
	if (strCmd.compare("connect_complete") == 0)
		::KillTimer(m_hWnd, IPC_TIMEOUT);
	else if (strCmd.compare("video_play") == 0)
		Play();
	else if (strCmd.compare("video_stop") == 0)
		::ShowWindow(m_hWnd, SW_HIDE);//Stop();

	return true;
}

void CVideoWnd::OnChannelConnected(int32 peer_pid)
{
	Send("launch");
}

void CVideoWnd::OnChannelError()
{
	Stop();
}

void CVideoWnd::Send(const std::string & cmd)
{
	scoped_refptr<IPC::Message> m(new IPC::Message(GetCurrentProcessId(), 0, (IPC::Message::PriorityValue)0));
	m->WriteString(cmd);
	if (m_pEndpoint)
		m_pEndpoint->Send(m.get());
}

void CVideoWnd::FuncPlayerEvent(void * user_data, const PLAYER_EVENT e, LPVOID pData)
{
	CVideoWnd * pAvPlayer = (CVideoWnd*)user_data;
	if (PlayerOpening == e)
		::PostMessage(pAvPlayer->GetHWND(), PLAYER_MSG_PLAY, (WPARAM)0, (LPARAM)0);
	else if (PlayerClosed == e)
		::PostMessage(pAvPlayer->GetHWND(), PLAYER_MSG_CLOSE, (WPARAM)0, (LPARAM)0);
	else if (PlayerError == e)
	{
		PlayerErrorSt* pSt = (PlayerErrorSt*)pData;
		pAvPlayer->m_pLabelMsg->SetText(CA2W(pSt->strErrMsg.c_str(), CP_UTF8));
	}
}



