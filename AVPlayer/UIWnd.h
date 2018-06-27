#pragma once

#include "../DuiLib/UIlib.h"
using namespace DuiLib;

// 将HWND显示到CControlUI上面
class CWndUI : public CControlUI
{
public:
	CWndUI() : m_hWnd(NULL) {}

	virtual void SetVisible(bool bVisible = true)
	{
		__super::SetVisible(bVisible);
		::ShowWindow(m_hWnd, bVisible ? SW_SHOW : SW_HIDE);
	}

	virtual void SetInternVisible(bool bVisible = true)
	{
		__super::SetInternVisible(bVisible);
		::ShowWindow(m_hWnd, bVisible ? SW_SHOW : SW_HIDE);
	}

	virtual void SetPos(RECT rc, bool bNeedInvalidate = true)
	{
		__super::SetPos(rc, bNeedInvalidate);
		if ( 0 != m_proportion )
		{
			int nWidth = rc.right - rc.left;
			int nHeight = rc.bottom - rc.top;
			if (nWidth > (nHeight * m_proportion))
			{
				nWidth = nHeight * m_proportion;
				nHeight = nHeight;
			}
			else
			{
				nWidth = nWidth;
				nHeight = nWidth / m_proportion;
			}
			int left = (rc.right - rc.left - nWidth) / 2;
			int top = (rc.bottom - rc.top - nHeight) / 2;
			MoveWindow(m_hWnd, rc.left + left, rc.top + top, nWidth, nHeight, true);
		}
		else
		{
			MoveWindow(m_hWnd, rc.left + 1, rc.top + 1, rc.right - rc.left - 2, rc.bottom - rc.top - 2, true);
		}
	}

	BOOL Attach(HWND hWndNew)
	{
		if (!::IsWindow(hWndNew))
		{
			return FALSE;
		}

		m_hWnd = hWndNew;
		RECT rc = __super::GetPos();
		MoveWindow(m_hWnd, rc.left + 1, rc.top + 1, rc.right - rc.left - 2, rc.bottom - rc.top - 2, true);
		bool bVisible = __super::IsVisible();
		::ShowWindow(m_hWnd, bVisible);
		return TRUE;
	}

	void SetProportion(double proportion)
	{
		m_proportion = proportion;
	}

	HWND Detach()
	{
		if (m_hWnd == NULL)
		{
			return NULL;
		}

		HWND hWnd = m_hWnd;
		m_hWnd = NULL;
		return hWnd;
	}

	HWND GetHWND()
	{
		return m_hWnd;
	}

protected:
	HWND m_hWnd;
	double m_proportion = 0.0;
};