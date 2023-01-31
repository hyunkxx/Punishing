#include "..\Public\Object_Manager.h"
#include "Layer.h"

IMPLEMENT_SINGLETON(CObject_Manager)

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iLevelMaxCount)
{
	if (nullptr != m_pLayers)
		return E_FAIL;

	m_pLayers = new LAYERS[iLevelMaxCount];
	m_iLevelMaxCount = iLevelMaxCount;

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const _tchar* pPrototypeTag, CGameObject* pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
		return E_FAIL;

	m_Prototypes.emplace(pPrototypeTag, pPrototype);
	return S_OK;
}

HRESULT CObject_Manager::Add_GameObject(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pLayerTag, void* pArg)
{
	CGameObject*pPrototype = Find_Prototype(pPrototypeTag);
	if (nullptr == pPrototype)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);
	if (nullptr == pGameObject)
		return E_FAIL;

	CLayer*	pLayer = Find_Layer(iLevelIndex, pLayerTag);
	if (nullptr == pLayer)
	{
		pLayer = CLayer::Create();
		pLayer->Add_GameObject(pGameObject);
		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
	{
		pLayer->Add_GameObject(pGameObject);
	}

	return S_OK;
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->Tick(TimeDelta);
		}
	}
}

void CObject_Manager::LateTick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			pair.second->LateTick(TimeDelta);
		}
	}
}

void CObject_Manager::Clear(_uint iLevelIndex)
{
	for (auto& pair : m_pLayers[iLevelIndex])
	{
		Safe_Release(pair.second);
	}
	m_pLayers[iLevelIndex].clear();
}

CGameObject* CObject_Manager::Find_Prototype(const _tchar* pPrototypeTag)
{
	auto iter = find_if(m_Prototypes.begin(), m_Prototypes.end(), CTagFinder(pPrototypeTag));
	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer* CObject_Manager::Find_Layer(_uint iLevelIndex, const _tchar* pLayerTag)
{
	if (iLevelIndex >= m_iLevelMaxCount)
		return nullptr;

	auto iter = find_if(m_pLayers[iLevelIndex].begin(), m_pLayers[iLevelIndex].end(), CTagFinder(pLayerTag));
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iLevelMaxCount; ++i)
	{
		for (auto& pair : m_pLayers[i])
		{
			Safe_Release(pair.second);
		}
		m_pLayers[i].clear();
	}
	Safe_Delete_Array(m_pLayers);

	for (auto& pair : m_Prototypes)
		Safe_Release(pair.second);

	m_Prototypes.clear();
}
