#include "pch.h"
#include "..\Public\WarningImage.h"

#include "GameInstance.h"

CWarningImage::CWarningImage(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CWarningImage::CWarningImage(const CWarningImage & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWarningImage::Initialize_Prototype()
{
	if (FAILED(CGameObject::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWarningImage::Initialize(void * pArg)
{
	if (FAILED(CGameObject::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	 
	m_pTransform->Set_Scale(_float3(6.5f, 1.f, 1.f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(30.f, 1.f, 21.f, 1.f));
	
	return S_OK;
}

void CWarningImage::Tick(_double TimeDelta)
{
	CGameObject::Tick(TimeDelta);
	
}

void CWarningImage::LateTick(_double TimeDelta)
{
	CGameObject::LateTick(TimeDelta);

	if (!m_bActive)
		return;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CPipeLine* pPipeline = CPipeLine::GetInstance();
	_float4x4 InversViewMatrix = pPipeline->Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW);
	_float3 vScale = m_pTransform->Get_Scale();
	_float4x4 worldMatrix = m_pTransform->Get_WorldMatrix();
	InversViewMatrix._41 = worldMatrix._41;
	InversViewMatrix._42 = worldMatrix._42;
	InversViewMatrix._43 = worldMatrix._43;
	m_pTransform->Set_WorldMatrix(InversViewMatrix);
	m_pTransform->Set_Scale(vScale);

	_vector vPos = m_pOwnerTransform->Get_State(CTransform::STATE_POSITION);
	vPos = XMVectorSetY(vPos, m_fHeight);
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);

	if (m_fAlpha <= 0.f)
	{
		m_bActive = false;
		m_fAlpha = 1.f;
		m_fCurrentScaleY = m_fOriginScaleY;
		return;
	}

	m_fAlpha -= TimeDelta;
	m_fCurrentScaleY -= TimeDelta;

	if (nullptr != m_pRenderer)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_EFFECT, this);
}

HRESULT CWarningImage::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Setup_ShaderResources()))
		return E_FAIL;

	m_pTexture->Setup_ShaderResource(m_pShader, "g_Texture");

	if (FAILED(m_pShader->SetRawValue("g_TimeAcc", &m_fAlpha, sizeof(float))))
		return E_FAIL;

	m_pShader->Begin(12);
	m_pVIBuffer->Render();

	return S_OK;
}

void CWarningImage::RenderGUI()
{
}

void CWarningImage::SetStartEffect(CTransform * pOwnerTransform, float fHeight)
{
	m_bActive = true;
	m_fHeight = fHeight;

	m_pTransform->Set_Scale(_float3(10.f, 1.f, 1.f));
	m_pOwnerTransform = pOwnerTransform;
}

HRESULT CWarningImage::Add_Components()
{
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));
	TransformDesc.fMoveSpeed = 5.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.0f);
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"),
		TEXT("transform_main"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("vibuffer_rect"), (CComponent**)&m_pVIBuffer)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"),
		TEXT("shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_warning"),
		TEXT("texture1"), (CComponent**)&m_pTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWarningImage::Setup_ShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	//m_pTransform->Set_State(CTransform::STATE_RIGHT, m_pTransform->Get_State(CTransform::STATE_RIGHT) * vScale.x);
	//m_pTransform->Set_State(CTransform::STATE_UP, m_pTransform->Get_State(CTransform::STATE_UP) * vScale.y);
	//m_pTransform->Set_State(CTransform::STATE_LOOK, m_pTransform->Get_State(CTransform::STATE_LOOK) * vScale.z);
	
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pTransform->Get_WorldMatrix())))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CWarningImage * CWarningImage::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWarningImage* pInstance = new CWarningImage(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWarningImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWarningImage::Clone(void * pArg)
{
	CWarningImage* pInstance = new CWarningImage(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWarningImage");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWarningImage::Free()
{
	__super::Free();

	Safe_Release(m_pTransform);
	Safe_Release(m_pShader);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTexture);
}
