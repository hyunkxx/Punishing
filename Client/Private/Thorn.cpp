#include "pch.h"
#include "..\Public\Thorn.h" 
 

#include "GameInstance.h"
#include "Character.h"
#include "Bone.h"

CThorn::CThorn(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CThorn::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CThorn::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	return S_OK;
}

void CThorn::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_bScaleUp)
		ScaleUpProcess(TimeDelta);

	if (m_bMove)
		MoveProcess(TimeDelta);
}

void CThorn::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	_float fLength = GetLengthFromCamera();

	if (m_eThornType == THORN)
	{
		if (fLength < 5.f)
		{
			m_bAlpha = true;
			if (nullptr != m_pRenderer && m_bRender)
				m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
		}
		else
		{
			m_bAlpha = false;
			if (nullptr != m_pRenderer && m_bRender)
				m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
		}
	}
	else
	{
		m_bAlpha = false;
		if (nullptr != m_pRenderer && m_bRender)
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
}

HRESULT CThorn::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = m_pModel->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		m_pModel->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);

		if(m_bAlpha)
			m_pShader->Begin(3);
		else
			m_pShader->Begin(2);

		m_pModel->Render(i);
	}

	return S_OK;
}

void CThorn::RenderGUI()
{
}

_float CThorn::GetLengthFromCamera()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));
	
	return fLength;
}

void CThorn::Reset()
{
	m_bMove = false;
	m_bScaleUp = false;
	m_bScaleFinish = false;
	m_fLength = 1.f;
	m_fScaleAcc = 0.f;
	_float3 vAngle = { 0.f, 0.f, 0.f };
	//m_pTransform->SetRotationXYZ(vAngle);
	m_pTransform->Set_Scale(m_fPrevScale);
}

void CThorn::SetPosition(_fvector vPos)
{
	if(m_pTransform)
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
}

void CThorn::SetRotationXYZ(_float3 vAngle)
{
	if (m_pTransform)
		m_pTransform->SetRotationXYZ(vAngle);
}

void CThorn::SetRotationToTarget(_fvector vTargetDir)
{
	_vector vDir = XMVector3Normalize(vTargetDir);
	//_vector vUp = m_pTransform->Get_State(CTransform::STATE_UP);
	//_vector vPos = XMVectorSetY(m_pTransform->Get_State(CTransform::STATE_POSITION), 0.f);
	//_vector vDir = XMVector3Normalize(vTarget - vPos);

	m_pTransform->Set_State(CTransform::STATE_UP, vDir);
	m_pTransform->Set_State(CTransform::STATE_LOOK, -VECTOR_UP);

	_vector vRight = XMVector3Cross(m_pTransform->Get_State(CTransform::STATE_UP), -VECTOR_UP);
	m_pTransform->Set_State(CTransform::STATE_RIGHT, vRight);
}

void CThorn::SetupScaleUpStart(_float fLength)
{
	m_bScaleUp = true;
	m_bRender = true;
	m_fLength = fLength;
}

_bool CThorn::ScaleUpProcess(_double TimeDelta)
{
	_float vRightLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_RIGHT)));
	_float vUpLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_UP)));
	_float vLookLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_LOOK)));

	_float3 vLength;
	if (m_eThornType == THORN)
	{
		m_fScaleAcc += TimeDelta;
		vLength.x = vRightLength + powf(vRightLength, m_fScaleAcc);
		vLength.y = vUpLength + powf(vUpLength, m_fScaleAcc);
		vLength.z = vLookLength + powf(vLookLength, m_fScaleAcc);
	}
	else if (m_eThornType == MISSILE)
	{
		vLength.x = vRightLength + TimeDelta * 2.f;
		vLength.y = vUpLength + TimeDelta * 2.f;
		vLength.z = vLookLength + TimeDelta * 2.f;
	}

	m_pTransform->Set_Scale(vLength);

	if (vLength.y >= m_fLength)
	{
		m_bScaleUp = false;
		m_bScaleFinish = true;
		return true;
	}
	else
		return false;
}

_bool CThorn::MoveProcess(_double TimeDelta)
{
	//Up방향으로 이동
	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vUp = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_UP));
	vPos = vPos + vUp * TimeDelta;
	
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);

	//m_fMoveAcc += TimeDelta;
	/*if (m_fMoveAcc > 10.f);
	{
		m_fMoveAcc = 0.f;
		Reset();
	}*/

	return true;
}

HRESULT CThorn::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"), TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_BOSS, TEXT("proto_com_model_thorn"), TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CThorn::SetupShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (FAILED(m_pTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vCamPosition", &pInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	const LIGHT_DESC* LightDesc = pInstance->GetLightDesc(0);
	if (nullptr == LightDesc)
		return E_FAIL;

	if (FAILED(m_pShader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vLightDiffuse", &LightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vLightSpecular", &LightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShader->SetRawValue("g_vLightAmbient", &LightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CThorn * CThorn::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CThorn*	pInstance = new CThorn(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CThorn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CThorn::Clone(void * pArg)
{
	CThorn* pInstance = new CThorn(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CThorn");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CThorn::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);

}