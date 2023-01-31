#pragma once

#include "Base.h"
#include "Timer.h"

BEGIN(Engine)

class CTimer_Manager : public CBase
{
	DECLARE_SINGLETON(CTimer_Manager)

private:
	explicit CTimer_Manager();
	virtual ~CTimer_Manager() = default;

public:
	HRESULT	CreateTimer(const _tchar* pTimerTag);
	_float	GetTimer(const _tchar* pTimerTag);
	void	SetTimer(const _tchar* pTimerTag);
	
private:
	CTimer*	FindTimer(const _tchar* pTimerTag);

public:
	virtual	void Free(void) override;
	      
private:
	unordered_map<const _tchar*, CTimer*> m_umapTimers;

};

END
