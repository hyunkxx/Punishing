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
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject.second)
			pGameObject.second->Tick(TimeDelta);
	}
}

void CLayer::LateTick(_double TimeDelta)
{
 	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject.second)
			pGameObject.second->LateTick(TimeDelta);
	}
}

void CLayer::RenderGUI()
{
	for (auto& pGameObject : m_GameObjects)
	{
		if (nullptr != pGameObject.second)
			pGameObject.second->RenderGUI();
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
