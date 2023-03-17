#pragma once

#include "Base.h"

BEGIN(Client)

class CApplicationManager final : public CBase
{
	DECLARE_SINGLETON(CApplicationManager)

private:
	CApplicationManager();
	virtual ~CApplicationManager() = default;

public:
	void SetTitle(wstring strTitle);

	//초산공간 오픈
	void SetFreeze(_bool value) { m_bFreeze = value; }
	_bool IsFreeze() const { return m_bFreeze; }
	void SetPlayerPrevPosition(_float3 value) { m_vPrevPosition = value; }
	_vector GetPlayerPrevPosition() { return XMLoadFloat3(&m_vPrevPosition); }

	void SetHitFreeze(_bool value) { m_bHitFreeze = value; }
	_bool IsHitFreeze() const { return m_bHitFreeze; }
public:
	virtual void Free() override;

private:
	std::wstring m_strTitleText;
	_bool m_bFreeze = false;
	_float3 m_vPrevPosition = { 0.f, 0.f, 0.f };

	_bool m_bHitFreeze = false;
};

END