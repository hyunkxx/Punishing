#include "pch.h"
#include "..\Public\ApplicationManager.h"

IMPLEMENT_SINGLETON(CApplicationManager)

CApplicationManager::CApplicationManager()
{
}

void CApplicationManager::SetTitle(wstring strTitle)
{
	if (m_strTitleText != strTitle)
	{
		m_strTitleText = strTitle;
		SetWindowText(g_hWnd, m_strTitleText.c_str());
	}
}

void CApplicationManager::Free()
{
}
