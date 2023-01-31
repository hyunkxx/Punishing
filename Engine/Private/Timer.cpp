#include "Timer.h"

USING(Engine)

CTimer::CTimer()
	: m_fTimeDelta(0.f)
{
	ZeroMemory(&m_tFrameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tFixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tLastTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}


HRESULT CTimer::Initalize(void)
{
	QueryPerformanceCounter(&m_tFrameTime);
	QueryPerformanceCounter(&m_tLastTime);
	QueryPerformanceCounter(&m_tFixTime);
	QueryPerformanceFrequency(&m_CpuTick);

	return S_OK;
}

void Engine::CTimer::Update(void)
{
	QueryPerformanceCounter(&m_tFrameTime);

	if (m_tFrameTime.QuadPart - m_tFixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_tFixTime = m_tFrameTime;
	}

	m_fTimeDelta = ((m_tFrameTime.QuadPart) - (m_tLastTime.QuadPart)) / _float(m_CpuTick.QuadPart);

	m_tLastTime = m_tFrameTime;
}

CTimer * CTimer::Create(void)
{
	CTimer *	pInstance = new CTimer;
	if (FAILED(pInstance->Initalize()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTimer::Free(void)
{
}
