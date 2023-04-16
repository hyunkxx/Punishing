#include "pch.h"
#include "..\Public\Cloud.h"

#include "GameInstance.h"

CCloud::CCloud(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSprite(pDevice, pContext)
{
}

CCloud::CCloud(const CSprite & rhs)
	: CSprite(rhs)
{
}

HRESULT CCloud::Initialize_Prototype()
{
	if (FAILED(CSprite::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CCloud::Initialize(void * pArg)
{
	if (FAILED(CSprite::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	CSprite::SetSpeed(0.03f);
	CSprite::SetupSprite(m_pDiffuseTexture, _float2(6.f, 3.f), SPRITE_PLAY::ONE);

	_vector vPos = XMVectorSet(30.f, 1.f, 25.f, 1.f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransform->Set_Scale(_float3(1.5f, 1.5f, 0.f));

	XMStoreFloat4(&m_vOriginPos, m_pTransform->Get_State(CTransform::STATE_POSITION));
	ZeroMemory(m_vPos, sizeof(_float3) * 5);

	return S_OK;
}

void CCloud::Tick(_double TimeDelta)
{
	CSprite::Tick(TimeDelta);
}

void CCloud::LateTick(_double TimeDelta)
{
	CSprite::LateTick(TimeDelta);

	if (!m_bRender)
		return;

	_vector vLook = -XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK));
	_vector vRight = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_RIGHT));

	//XMStoreFloat4(&m_vPos[0], XMLoadFloat4(&m_vOriginPos) - vLook);
	//XMStoreFloat4(&m_vPos[1], XMLoadFloat4(&m_vOriginPos) - XMVector3Normalize(vLook - vRight));
	//XMStoreFloat4(&m_vPos[2], XMLoadFloat4(&m_vOriginPos) - XMVector3Normalize(vLook + vRight));
	XMStoreFloat4(&m_vPos[0], XMLoadFloat4(&m_vOriginPos) + -vRight * 2.f);
	XMStoreFloat4(&m_vPos[1], XMLoadFloat4(&m_vOriginPos) + vRight * 2.f);
	XMStoreFloat4(&m_vPos[2], XMLoadFloat4(&m_vOriginPos) + XMVector3Normalize(vLook -vRight) * 2.f);
	XMStoreFloat4(&m_vPos[3], XMLoadFloat4(&m_vOriginPos) + XMVector3Normalize(vLook + vRight) * 2.f);
	XMStoreFloat4(&m_vPos[4], XMLoadFloat4(&m_vOriginPos) + vLook * 2.f);
}

HRESULT CCloud::Render()
{
	for (int i = 0; i < 5; ++i)
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&m_vPos[i]));
		CSprite::SetupSpriteShaderResource(SPRITE_TYPE::BILLBOARD);
		m_pSpriteShader->Begin(1);//패스1번 마스크이미지로 렌더
		CSprite::Render();
	}

	return S_OK;
}

void CCloud::RenderGUI()
{
}

//_float CCloud::GetLengthFromCamera()
//{
//	CPipeLine* pPipeLine = CPipeLine::GetInstance();
//
//	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
//	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());
//
//	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));
//
//	return fLength;
//}

void CCloud::StartEffect(_fvector vPosition)
{
	m_bRender = true;
	SetPosition(vPosition);
}

void CCloud::SetPosition(_fvector vPosition)
{
	_vector vFixUpPosition = vPosition;
	vFixUpPosition = XMVectorSetY(vPosition, 0.6f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vFixUpPosition);
	XMStoreFloat4(&m_vOriginPos, vFixUpPosition);
}

HRESULT CCloud::AddComponent()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_texture_cloud", L"com_texture_sprite", (CComponent**)&m_pDiffuseTexture)))
		return E_FAIL;

	return S_OK;
}

CCloud * CCloud::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CCloud*pInstance = new CCloud(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CCloud::Clone(void * pArg)
{
	CCloud* pInstance = new CCloud(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CCloud");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCloud::Free()
{
	__super::Free();

	Safe_Release(m_pDiffuseTexture);

}
