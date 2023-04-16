#include "pch.h"
#include "..\Public\DashSprite.h"

#include "GameInstance.h"

CDashSprite::CDashSprite(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CSprite(pDevice, pContext)
{
}

CDashSprite::CDashSprite(const CSprite & rhs)
	: CSprite(rhs)
{
}

HRESULT CDashSprite::Initialize_Prototype()
{
	if (FAILED(CSprite::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CDashSprite::Initialize(void * pArg)
{
	if (FAILED(CSprite::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	CSprite::SetSpeed(0.02f);
	CSprite::SetupSprite(m_pLeftDustTexture, _float2(3.f, 8.f), SPRITE_PLAY::ONE);
	CSprite::SetupSprite(m_pRightDustTexture, _float2(3.f, 8.f), SPRITE_PLAY::ONE);

	_vector vPos = XMVectorSet(30.f, 1.f, 25.f, 1.f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransform->Set_Scale(_float3(5.f, 1.f, 0.f));

	m_bRender = false;

	return S_OK;
}

void CDashSprite::Tick(_double TimeDelta)
{
	CSprite::Tick(TimeDelta);
}

void CDashSprite::LateTick(_double TimeDelta)
{
	if (!m_bRender)
		return;

	CSprite::LateTick(TimeDelta);
}

HRESULT CDashSprite::Render()
{
	CSprite::SetupSpriteShaderResource(SPRITE_TYPE::NONE);

	m_pRightFootTransform->Setup_ShaderResource(m_pSpriteShader, "g_WorldMatrix");
	m_pRightDustTexture->Setup_ShaderResource(m_pSpriteShader, "g_DiffuseTexture");
	m_pSpriteShader->Begin(2);
	CSprite::Render();

	m_pLeftFootTransform->Setup_ShaderResource(m_pSpriteShader, "g_WorldMatrix");
	m_pLeftDustTexture->Setup_ShaderResource(m_pSpriteShader, "g_DiffuseTexture");
	m_pSpriteShader->Begin(2);
	CSprite::Render();

	return S_OK;
}

void CDashSprite::RenderGUI()
{
}

//_float CDashSprite::GetLengthFromCamera()
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

void CDashSprite::StartEffect(CTransform* pTransform)
{
	_float3 vScale = m_pTransform->Get_Scale();

	_vector vRight = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_RIGHT));
	_vector vLook = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_LOOK));
	_vector vPivotPos = pTransform->Get_State(CTransform::STATE_POSITION) + vLook;
	
	_vector vFixLook = XMVector3Normalize(vLook + (vRight * 3.5f));
	_vector vFixRight = XMVector3Normalize(XMVector3Cross(VECTOR_UP, vFixLook)) * vScale.x;

	_vector vFixPos = vPivotPos + vRight;
	vFixPos = XMVectorSetY(vFixPos, 0.5f);
	
	m_pRightFootTransform->Set_State(CTransform::STATE_LOOK, vFixLook);
	m_pRightFootTransform->Set_State(CTransform::STATE_UP, VECTOR_UP);
	m_pRightFootTransform->Set_State(CTransform::STATE_RIGHT, vFixRight);
	m_pRightFootTransform->Set_State(CTransform::STATE_POSITION, vFixPos);

	vFixLook = XMVector3Normalize(vLook - (vRight * 3.5f));
	vFixRight = XMVector3Normalize(XMVector3Cross(VECTOR_UP, vFixLook)) * vScale.x;
	vFixPos = vPivotPos - vRight;
	vFixPos = XMVectorSetY(vFixPos, 0.5f);

	m_pLeftFootTransform->Set_State(CTransform::STATE_LOOK, vFixLook);
	m_pLeftFootTransform->Set_State(CTransform::STATE_UP, VECTOR_UP);
	m_pLeftFootTransform->Set_State(CTransform::STATE_RIGHT, vFixRight);
	m_pLeftFootTransform->Set_State(CTransform::STATE_POSITION, vFixPos);

	m_bRender = true;
}

HRESULT CDashSprite::AddComponent()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_texture_leftdust", L"com_texture_sprite0", (CComponent**)&m_pLeftDustTexture)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_texture_rightdust", L"com_texture_sprite1", (CComponent**)&m_pRightDustTexture)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_transform", L"com_lefttransform", (CComponent**)&m_pLeftFootTransform)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_transform", L"com_righttransform", (CComponent**)&m_pRightFootTransform)))
		return E_FAIL;

	return S_OK;
}

CDashSprite * CDashSprite::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CDashSprite*pInstance = new CDashSprite(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CDashSprite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CDashSprite::Clone(void * pArg)
{
	CDashSprite* pInstance = new CDashSprite(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CDashSprite");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDashSprite::Free()
{
	__super::Free();

	Safe_Release(m_pLeftDustTexture);
	Safe_Release(m_pRightDustTexture);

}
