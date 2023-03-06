#include "..\Public\GameObject.h"

#include "GameInstance.h"

_uint CGameObject::ObjectID = 0;

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject& rhs)
	: m_pDevice{ rhs.m_pDevice }
	, m_pContext{ rhs.m_pContext }
{
	ObjectID++;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

void CGameObject::Tick(_double TimeDelta)
{
}

void CGameObject::LateTick(_double TimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::RenderGUI()
{
	return;
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CComponent* pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	*ppOut = pComponent;
	m_Components.emplace(pComponentTag, pComponent);
	Safe_AddRef(pComponent);

	return S_OK;
}

CComponent* CGameObject::Find_Component(const _tchar* pComponentTag)
{
	if (this == nullptr)
		return nullptr;

	auto iter = find_if(m_Components.begin(), m_Components.end(), CTagFinder(pComponentTag));
	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& pair : m_Components)
	{
		Safe_Release(pair.second);
	}
	m_Components.clear();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);
}