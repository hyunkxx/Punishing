#include "Timer_Manager.h"

USING(Engine)
IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

_float Engine::CTimer_Manager::GetTimer(const _tchar* pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return 0.f;

	return pTimer->Get_TimeDelta();
}

void CTimer_Manager::SetTimer(const _tchar * pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);
	if (nullptr == pTimer)
		return;

	pTimer->Update();
}

CTimer* Engine::CTimer_Manager::FindTimer(const _tchar* pTimerTag)
{
	auto iter = find_if(m_umapTimers.begin(), m_umapTimers.end(), CTagFinder(pTimerTag));
	if (iter == m_umapTimers.end())
		return nullptr;

	return iter->second;
}

HRESULT Engine::CTimer_Manager::CreateTimer(const _tchar* pTimerTag)
{
	CTimer*	pTimer = FindTimer(pTimerTag);

	assert(nullptr == pTimer);
	if (nullptr != pTimer)
		return E_FAIL;

	pTimer = CTimer::Create();

	assert(nullptr != pTimer);
	if (nullptr == pTimer)
		return E_FAIL;

	m_umapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

void Engine::CTimer_Manager::Free(void)
{
	for (auto& Pair : m_umapTimers)
		Safe_Release(Pair.second);

	m_umapTimers.clear();
}
