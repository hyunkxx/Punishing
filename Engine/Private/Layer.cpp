#include "..\Public\Layer.h"

CLayer::CLayer()
{
}

//HRESULT CLayer::Add_GameObject(CGameObject* pGameObject)
//{
//	if (nullptr == pGameObject)
//		return E_FAIL;
//
//	m_GameObjects.emplace(pGameObject);
//
//	return S_OK;
//}

HRESULT CLayer::Add_GameObject(wstring strObjectTag, CGameObject* pGameObject)
{
	if (nullptr == pGameObject)
		return E_FAIL;

	m_GameObjects.emplace(strObjectTag, pGameObject);

	return S_OK;
}

void CLayer::Tick(_double TimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != iter->second)
		{
			if (iter->second->IsActive())
				iter->second->Tick(TimeDelta);

			iter++;
		}
	}
}

void CLayer::LateTick(_double TimeDelta)
{
	for (auto iter = m_GameObjects.begin(); iter != m_GameObjects.end();)
	{
		if (nullptr != iter->second)
		{
			if (iter->second->IsDestroy())
			{
				Safe_Release(iter->second);
				iter = m_GameObjects.erase(iter);
				continue;
			}

			if (iter->second->IsActive())
				iter->second->LateTick(TimeDelta);

			iter++;
		}
	}
}

void CLayer::RenderGUI()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject.second)
		{
			if(!pGameObject.second->IsDestroy())
				pGameObject.second->RenderGUI();
		}
	}
}

CGameObject* CLayer::Find_GameObject(wstring strObjectTag)
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (strObjectTag == pGameObject.first)
			return pGameObject.second;
	}

	return nullptr;
}

void CLayer::Clear()
{
	for (auto pair : m_GameObjects)
		Safe_Release(pair.second);

	m_GameObjects.clear();
}

CLayer* CLayer::Create()
{
	CLayer*	pInstance = new CLayer();

	return pInstance;
}

void CLayer::Free()
{
	for (auto& pGameObject : m_GameObjects)
		Safe_Release(pGameObject.second);

	m_GameObjects.clear();
}
