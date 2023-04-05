#include "pch.h"
#include "..\Public\SwordTrail.h"

#include "GameInstance.h"
#include "Character.h"
#include "Bone.h"

CSwordTrail::CSwordTrail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CSwordTrail::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSwordTrail::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;

	if (pArg != nullptr)
	{
		m_pPlayer = (CCharacter*)pArg;
		m_pPlayerTransform = (CTransform*)m_pPlayer->Find_Component(L"com_transform");
		_vector vPosition = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);
		m_pPivotBone = m_pPlayer->GetBone("Bip001");
	}
	else
		return E_FAIL;
	
	return S_OK;
}

void CSwordTrail::Tick(_double TimeDelta)
{
	if (!m_bUse)
		return;

	__super::Tick(TimeDelta);

	m_fTimeAcc += TimeDelta;
	if (m_fTimeAcc >= m_fTimeAccLimit)
	{
		m_bUse = false;
		m_bHit = false;
		m_fTimeAcc = 0.f;
		m_vMinScale.y = 1.f;
		m_pHitTransform->Set_Scale(m_vMinScale);
	}

	//if (m_bHit)
	//{
	//	m_vMinScale.y -= TimeDelta;
	//	m_pHitTransform->Set_Scale(m_vMinScale);
	//}
}

void CSwordTrail::LateTick(_double TimeDelta)
{
	if (!m_bUse)
		return;

	__super::LateTick(TimeDelta);

	if (nullptr != m_pRenderer && m_bUse)
		m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CSwordTrail::Render()
{
	if (!m_bUse)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;


	if (m_bHit)
	{
		if (FAILED(m_pHitTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
			return E_FAIL;

		_uint MeshCount = m_pModel2->Get_MeshCount();
		for (_uint i = 0; i < MeshCount; ++i)
		{
			if (FAILED(m_pDiffuse->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pDiffuse->Setup_ShaderResource(m_pShader, "g_MaskTexture")))
				return E_FAIL;

			m_pShader->Begin(1);
			m_pModel2->Render(i);
		}
	}

	_uint MeshCount = m_pModel->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		//m_pModel->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
		if (FAILED(m_pDiffuse->Setup_ShaderResource(m_pShader, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pDiffuse->Setup_ShaderResource(m_pShader, "g_MaskTexture")))
			return E_FAIL;

		m_pShader->Begin(0);
		m_pModel->Render(i);
	}



	return S_OK;
}

void CSwordTrail::RenderGUI()
{
}

HRESULT CSwordTrail::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform_hit"), (CComponent**)&m_pHitTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_swordtrail"), TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_sword_trail"), TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_sword_trail2"), TEXT("com_model2"), (CComponent**)&m_pModel2)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_sword_mask1"), TEXT("com_diffuse_texture"), (CComponent**)&m_pDiffuse)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_sword_mask2"), TEXT("com_mask_texture"), (CComponent**)&m_pMask)))
		return E_FAIL;


	return S_OK;
}

HRESULT CSwordTrail::SetupShaderResources()
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

	if (FAILED(m_pShader->SetRawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(_float))))
		return E_FAIL;

	return S_OK;

}

void CSwordTrail::Reset()
{
	m_fTimeAcc = 0.f; 
	m_pHitTransform->Set_Scale(_float3(1.f, 1.f, 1.f));
}

void CSwordTrail::EffectStart(_float3 vOffsetPos, _float3 fDegreeAngle)
{
	_float4x4 NewPivotMatrix = m_pPlayerTransform->Get_WorldMatrix();

	_vector vPlayerRight = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_RIGHT));
	_vector vPlayerUp = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_UP));
	_vector vPlayerLook = XMVector3Normalize(m_pPlayerTransform->Get_State(CTransform::STATE_LOOK));

	_vector vPlayerPos = m_pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	vPlayerPos = vPlayerPos + vPlayerRight * vOffsetPos.x;
	vPlayerPos = vPlayerPos + vPlayerUp * vOffsetPos.y;
	vPlayerPos = vPlayerPos + vPlayerLook * vOffsetPos.z;

	_float3 vPivotPos;
	XMStoreFloat3(&vPivotPos, vPlayerPos);
	
	NewPivotMatrix._41 = vPivotPos.x;
	NewPivotMatrix._42 = vPivotPos.y;
	NewPivotMatrix._43 = vPivotPos.z;

	_matrix RotX = XMMatrixRotationX(XMConvertToRadians(fDegreeAngle.x));
	_matrix RotY = XMMatrixRotationY(XMConvertToRadians(fDegreeAngle.y));
	_matrix RotZ = XMMatrixRotationZ(XMConvertToRadians(fDegreeAngle.z));

	XMStoreFloat4x4(&EffectWorldMatrix, RotZ * RotY * RotX * XMLoadFloat4x4(&NewPivotMatrix));
	m_pTransform->Set_WorldMatrix(EffectWorldMatrix);
	
	_vector whiteEffectPos = vPlayerPos + vPlayerLook * 2.5f;
	m_pHitTransform->Set_WorldMatrix(EffectWorldMatrix);
	m_pHitTransform->Set_State(CTransform::STATE_POSITION, whiteEffectPos);

	m_bUse = true;
}

void CSwordTrail::SetHitPosition(_vector vHitPos)
{
	//vHitPos = XMVectorSetY(vHitPos, 1.0f);
	//XMStoreFloat3(&m_vHitPos, vHitPos);

	//m_pHitTransform->Set_State(CTransform::STATE_POSITION, vHitPos);
	//m_pHitTransform->LookAt(XMLoadFloat4(&CPipeLine::GetInstance()->Get_CamPosition()));
	//m_pHitTransform->SetRotation(vLook, XMConvertToRadians(rand() % 20));
}

_float CSwordTrail::GetLengthFromCamera()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));

	return fLength;
}

CSwordTrail * CSwordTrail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSwordTrail*	pInstance = new CSwordTrail(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CSwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CSwordTrail::Clone(void * pArg)
{
	CSwordTrail* pInstance = new CSwordTrail(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CSwordTrail");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSwordTrail::Free()
{
	__super::Free();

	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pHitTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pModel2);
	Safe_Release(m_pShader);

}
