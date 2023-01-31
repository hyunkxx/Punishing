#pragma once

#include "Engine_Defines.h"

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
public:
	CBase() = default;
	virtual ~CBase() = default;

public:
	_ulong AddRef();
	_ulong Release();

public:
	virtual void Free() = 0;

private:
	_ulong m_dwRefCnt = { 0 };
};

END