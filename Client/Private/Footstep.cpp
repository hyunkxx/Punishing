#include "pch.h"
#include "..\Public\Footstep.h"

#include "GameInstance.h"

CFootstep::CFootstep(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSprite(pDevice, pContext)
{
}

CFootstep::CFootstep(const CSprite & rhs)
	: CSprite(rhs)
{
}

HRESULT CFootstep::Initialize_Prototype()
{
	if (FAILED(CSprite::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CFootstep::Initialize(void * pArg)
{
	if (FAILED(CSprite::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	CSprite::SetSpeed(0.05f);
	CSprite::SetupSprite(m_pDiffuseTexture, _float2(4.f, 4.f), SPRITE_PLAY::ONE);

	_vector vPos = XMVectorSet(30.f, 0.5f, 25.f, 1.f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransform->Set_Scale(_float3(0.5f, 0.5f, 0.f));
	
	return S_OK;
}

void CFootstep::Tick(_double TimeDelta)
{
	CSprite::Tick(TimeDelta);
}

void CFootstep::LateTick(_double TimeDelta)
{
	CSprite::LateTick(TimeDelta);

	if (!m_bRender)
		return;
}

HRESULT CFootstep::Render()
{
	CSprite::SetupSpriteShaderResource(SPRITE_TYPE::BILLBOARD);
	m_pSpriteShader->Begin(0);
	CSprite::Render();

	return S_OK;
}

void CFootstep::RenderGUI()
{
}

//_float CFootstep::GetLengthFromCamera()
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

void CFootstep::StartEffect(_fvector vPosition)
{
	m_bRender = true;
	SetPosition(vPosition);
}

void CFootstep::SetPosition(_fvector vPosition)
{
	_vector vFixUpPosition = vPosition;
	vFixUpPosition = XMVectorSetY(vPosition, 0.1f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vFixUpPosition);
}

HRESULT CFootstep::AddComponent()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_texture_foot", L"com_texture_sprite", (CComponent**)&m_pDiffuseTexture)))
		return E_FAIL;

	return S_OK;
}

CFootstep * CFootstep::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CFootstep*pInstance = new CFootstep(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CFootstep");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CFootstep::Clone(void * pArg)
{
	CFootstep* pInstance = new CFootstep(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CFootstep");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CFootstep::Free()
{
	__super::Free();

	Safe_Release(m_pDiffuseTexture);

}
