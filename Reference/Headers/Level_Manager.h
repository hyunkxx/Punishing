#pragma once

#include "Level.h"

BEGIN(Engine)

class ENGINE_DLL CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

private:
	CLevel_Manager() = default;
	virtual ~CLevel_Manager() = default;

public:
	HRESULT Open_Level(_uint iLevelIndex, CLevel* pCurrentLevel);
	void Tick_Level(_double TimeDelta);
public:
	virtual void Free() override;

private:
	_uint	m_iLevelIndex = { 0 };
	CLevel*	m_pCurrentLevel = { nullptr };

};

END