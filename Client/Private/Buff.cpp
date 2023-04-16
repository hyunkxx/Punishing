#include "pch.h"
#include "..\Public\Buff.h"

#include "GameInstance.h"

CBuff::CBuff(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CBuff::CBuff(const CBuff & rhs)
	: CGameObject(rhs)
{
}

HRESULT CBuff::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CBuff::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponent()))
		return E_FAIL;

	_vector vPos = XMVectorSet(30.f, 0.1f, 25.f, 1.f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	m_pTransform->Set_Scale(_float3(0.15f, 0.15f, 0.f));
	
	return S_OK;
}

void CBuff::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	
	if (!m_bRender)
		return;

	_vector vPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION) + XMLoadFloat4(&m_vDir);
	
	m_fAcc -= TimeDelta * m_fSpeed;
	m_fHeight += TimeDelta * ((m_fSpeed * 2.f) - (m_fAcc * 2.f));
	vPos = XMVectorSetY(vPos, m_fHeight);

	_float3 vScale = m_pTransform->Get_Scale();
	_float4 vCam = pGameInstance->Get_CamPosition();
	_vector vCamPos = XMLoadFloat4(&vCam);
	_vector vDir = XMVector3Normalize(vCamPos - m_pTransform->Get_State(CTransform::STATE_POSITION));

	_vector vRight = XMVector3Normalize(XMVector3Cross(VECTOR_UP, vDir));
	_vector vUp = VECTOR_UP;
	_vector vLook = XMVector3Normalize(XMVector3Cross(vRight, vUp));

	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight * 0.2f);
	m_pTransform->Set_State(CTransform::STATE_UP, vUp * 0.2f);
	m_pTransform->Set_State(CTransform::STATE_LOOK, vLook * 0.2f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_fAcc <= 0.f)
	{
		m_bRender = false;
		m_fAcc = m_fSpeed;
		m_fHeight = 0.0f;
		vPos = XMVectorSetY(vPos, 0.0f);
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}

}

void CBuff::LateTick(_double TimeDelta)
{
	if (!m_bRender)
		return;

	CGameObject::LateTick(TimeDelta);

	if (m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBuff::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CPipeLine* pPipeline = CPipeLine::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pTransform->Get_WorldMatrix())))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pPipeline->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pDiffuseTexture->Setup_ShaderResource(m_pShader, "g_Texture")))
		return E_FAIL;

	_float fAlpha = (m_fAcc / 1.5f) * 0.5f;
	if (FAILED(m_pShader->SetRawValue("g_TimeAcc", &fAlpha, sizeof(_float))))
		return E_FAIL;

	m_pShader->Begin(13);
	m_pVIBufferRect->Render();

	return S_OK;
}

void CBuff::RenderGUI()
{
}

//_float CBuff::GetLengthFromCamera()
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

void CBuff::StartEffect(_fvector vPosition, _float fSpeed, _float4 vDir)
{
	m_bRender = true;
	m_vDir = vDir;
	m_fSpeed = fSpeed;
	SetPosition(vPosition);
}

void CBuff::SetPosition(_fvector vPosition)
{
	_vector vFixUpPosition = vPosition;
	vFixUpPosition = XMVectorSetY(vPosition, 0.1f);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vFixUpPosition);
}

HRESULT CBuff::AddComponent()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_renderer", L"com_renderer", (CComponent**)&m_pRenderer)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_transform", L"com_transform", (CComponent**)&m_pTransform)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_shader_vtxtex", L"com_shader", (CComponent**)&m_pShader)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_vibuffer_rect", L"com_buffer", (CComponent**)&m_pVIBufferRect)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, L"proto_com_texture_triangle", L"com_texture_triangl", (CComponent**)&m_pDiffuseTexture)))
		return E_FAIL;

	return S_OK;
}

CBuff * CBuff::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBuff*pInstance = new CBuff(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBuff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBuff::Clone(void * pArg)
{
	CBuff* pInstance = new CBuff(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBuff");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBuff::Free()
{
	__super::Free();

	Safe_Release(m_pDiffuseTexture);

}
