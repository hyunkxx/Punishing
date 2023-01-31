#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CTimer : public CBase
{
private:
	explicit CTimer();
	virtual ~CTimer() = default;

public:
	_float Get_TimeDelta(void) { return m_fTimeDelta; }

public:
	HRESULT	Initalize(void);
	void	Update(void);

public:
	static CTimer* Create(void);
	virtual void   Free(void)override;

private:
	LARGE_INTEGER m_tFrameTime;
	LARGE_INTEGER m_tFixTime;
	LARGE_INTEGER m_tLastTime;
	LARGE_INTEGER m_CpuTick;
				  
	_float		  m_fTimeDelta;

};

END
