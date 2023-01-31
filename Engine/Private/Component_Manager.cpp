#include "..\Public\Component_Manager.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iLevelMaxCount)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iLevelMaxCount];
	m_iLevelMaxCount = iLevelMaxCount;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, CComponent* pPrototype)
{
	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent* CComponent_Manager::Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (nullptr == pPrototype)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(pArg);
	if (nullptr == pComponent)
		return nullptr;

	return pComponent;
}

CComponent* CComponent_Manager::Find_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag)
{
	if (nullptr == m_pPrototypes || iLevelIndex >= m_iLevelMaxCount)
		return nullptr;

	auto iter = find_if(m_pPrototypes[iLevelIndex].begin(), m_pPrototypes[iLevelIndex].end(), CTagFinder(pPrototypeTag));
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pPrototypes[i])
		{
			Safe_Release(pair.second);
		}
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
