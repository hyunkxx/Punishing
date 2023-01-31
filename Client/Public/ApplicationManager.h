#pragma once

#include "Base.h"

class CApplicationManager final : public CBase
{
	DECLARE_SINGLETON(CApplicationManager)

private:
	CApplicationManager();
	virtual ~CApplicationManager() = default;

public:
	void SetTitle(wstring strTitle);

public:
	virtual void Free() override;

private:
	std::wstring m_strTitleText;

};

