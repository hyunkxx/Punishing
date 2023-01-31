#include "..\Public\Base.h"

_ulong CBase::AddRef()
{
	return ++m_dwRefCnt;
}

_ulong CBase::Release()
{
	if (0 == m_dwRefCnt)
	{
		Free();
		delete this;

		return 0;
	}
	else
	{
		return m_dwRefCnt--;
	}
}
