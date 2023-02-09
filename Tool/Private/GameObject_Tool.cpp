#include "pch.h"
#include "..\Public\GameObject_Tool.h"

#include "GameInstance.h"

CGameObject_Tool::CGameObject_Tool(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_ObjectDesc, sizeof(OBJ_DESC));
}

CGameObject_Tool::CGameObject_Tool(const CGameObject_Tool & rhs)
	: CGameObject(rhs)
	, m_ObjectDesc(rhs.m_ObjectDesc)
{
}

HRESULT CGameObject_Tool::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGameObject_Tool::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 5.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(TOOL_STATIC, TEXT("proto_com_transform"),	TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	m_pTransform->Set_TransformDesc(TransformDesc);

	if (nullptr != pArg)
	{
	}

	return S_OK;
}

void CGameObject_Tool::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

}

void CGameObject_Tool::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);
}

HRESULT CGameObject_Tool::Render()
{
	return S_OK;
}

void CGameObject_Tool::RenderGUI()
{
}

void CGameObject_Tool::ApplyObjectDesc(const OBJ_DESC & objDesc)
{
	m_ObjectDesc.eType = OBJ_TYPE::OBJ_CUBE;
	m_pTransform->Set_Scale(m_ObjectDesc.vScale);
	m_pTransform->SetRotationXYZ(m_ObjectDesc.vRotation);
	m_pTransform->Set_State(CTransform::STATE_POSITION,
		XMVectorSet(m_ObjectDesc.vPosition.x, m_ObjectDesc.vPosition.y, m_ObjectDesc.vPosition.z, 1.f));
}

void CGameObject_Tool::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
}
