#include "pch.h"
#include "..\Public\Thorn.h" 
 

#include "GameInstance.h"
#include "ApplicationManager.h"
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
	ObjectID = 2;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;

	m_pFloorEffectTransform->SetRotation(VECTOR_RIGHT, XMConvertToRadians(90.f));
	m_pFloorEffectTransform->Set_Scale(_float3(3.5f, 1.f, 3.5f));
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pFloorGardTransform->Set_Scale(_float3(0.7f, 0.3f, 0.7f));
	m_pTrailTransform->Set_Scale(vTrailScale);

	return S_OK;
}

void CThorn::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::Tick(m_FixedTimeDelta);

	if (m_bGardRender)
	{
		m_fTimeAcc += m_FixedTimeDelta * 2.f;
		if (m_fTimeAcc >= 2.f)
		{
			m_fTimeAcc = 0.f;
			m_bGardRender = false;
		}
	}

	if (m_bFloorRender)
	{
		m_fFloorAlphaAcc += m_FixedTimeDelta;
		if (m_fFloorAlphaAcc >= 4.f)
		{
			m_fFloorAlphaAcc = 0.f;
			m_bFloorRender = false;
		}
	}

	m_pFloorEffectTransform->Rotate(VECTOR_UP, m_FixedTimeDelta);

	if (m_bScaleUp)
		ScaleUpProcess(m_FixedTimeDelta);

	if (m_bScaleUpDown)
		ScaleUpProcess(m_FixedTimeDelta);

	if (m_bScaleSmoothUp)
		ScaleUpSmoothProcess(m_FixedTimeDelta);
	
	if (m_bScaleSmoothDown)
		ScaleDownSmoothProcess(m_FixedTimeDelta);

	if (m_bMove)
		MoveProcess(m_FixedTimeDelta);

}

void CThorn::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (IsScaleFinish() && m_eThornType == MISSILE)
		pGameInstance->AddCollider(m_pCollider);
	if (IsScaleFinish() && m_eThornType == MISSILE)
		m_pCollider->Update(XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix()));

	__super::LateTick(m_FixedTimeDelta);

	_float fLength = GetLengthFromCamera();

	if (m_eThornType == THORN)
	{
		//if (fLength < 5.f)
		//	m_bAlpha = true;
		//else
		//	m_bAlpha = false;

		if (nullptr != m_pRenderer && m_bRender || m_bGardRender || m_bFloorRender)
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	}
	else
	{
		m_bAlpha = false;
		if (nullptr != m_pRenderer && m_bRender)
			m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
	}
}

HRESULT CThorn::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();


	if (m_eThornType == THORN)
	{
		//바닥 텍스쳐 이펙트
		if (m_bFloorRender)
		{
			//바닥 이펙트
			if (FAILED(m_pFloorShader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
				return E_FAIL;
			if (FAILED(m_pFloorShader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
				return E_FAIL;
			m_pFloorEffectTransform->Setup_ShaderResource(m_pFloorShader, "g_WorldMatrix");
			m_pFloorEffectDiffuse->Setup_ShaderResource(m_pFloorShader, "g_Texture");
			m_pFloorShader->SetRawValue("g_TimeAcc", &m_fFloorAlphaAcc, sizeof(float));

			//BS_AlphaBlend & DS_ZTest_NoZWrite
			m_pFloorShader->Begin(5);
			m_pFloorEffectBuffer->Render();
		}

		//모델 이펙트
		if (m_bGardRender)
		{
			if (FAILED(SetupShaderResources()))
				return E_FAIL;
			m_pFloorGardTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix");
			m_pFloorGardTexture->Setup_ShaderResource(m_pShader, "g_DiffuseTexture");
			m_pShader->SetRawValue("g_fTimeAcc", &m_fTimeAcc, sizeof(float));

			//BS_AlphaBlend & DS_ZTest_NoZWrite
			m_pShader->Begin(6);
			m_pFloorGard->Render(0);
		}
	}
	else
	{
		if (m_bMove)
		{
			//트레일 이펙트
			if (FAILED(m_pTrailEffectShader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
				return E_FAIL;
			if (FAILED(m_pTrailEffectShader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
				return E_FAIL;

			m_pTrailTransform->Setup_ShaderResource(m_pTrailEffectShader, "g_WorldMatrix");
			m_pTrailTexture->Setup_ShaderResource(m_pTrailEffectShader, "g_Texture");

			m_pTrailEffectShader->Begin(0);
			m_pTrailEffectBuffer->Render();
		}
	} 

	if (m_bRender)
	{
		if (FAILED(SetupShaderResources()))
			return E_FAIL;
		if (FAILED(m_pTransform->Setup_ShaderResource(m_pShader, "g_WorldMatrix")))
			return E_FAIL;
		_uint MeshCount = m_pModel->Get_MeshCount();
		for (_uint i = 0; i < MeshCount; ++i)
		{
			m_pModel->Setup_ShaderMaterialResource(m_pShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);

			if (CApplicationManager::GetInstance()->IsFreeze())
			{
				if(m_bAlpha)
					m_pShader->Begin(10);
				else
					m_pShader->Begin(8);
			}
			else
			{
				if (m_bAlpha)
					m_pShader->Begin(9);
				else
					m_pShader->Begin(2);
			}

			m_pModel->Render(i);
		}
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
	m_bRender = false;
	m_bScaleUpDown = false;

	m_bMove = false;
	m_bScaleUp = false;

	m_bScaleFinish = false;

	m_bScaleDownFinish = false;
	m_bScaleSmoothDown = false;

	m_fLength = 1.f;
	m_fScaleAcc = 0.01f;

	m_pTransform->Set_Scale(m_fPrevScale);
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	m_pTrailTransform->Set_Scale(vTrailScale);
	m_pTrailTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
}

void CThorn::SetPosition(_fvector vPos)
{
	_vector pos = vPos;
	if (m_pTransform)
	{
		m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
		pos = XMVectorSetY(vPos, XMVectorGetY(vPos) + 0.1f);
		m_pFloorEffectTransform->Set_State(CTransform::STATE_POSITION, pos);
		m_pFloorGardTransform->Set_State(CTransform::STATE_POSITION, pos);
		m_pTrailTransform->Set_State(CTransform::STATE_POSITION, pos);
	}

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

	m_pTrailTransform->Set_State(CTransform::STATE_UP, vDir * vTrailScale.y);
	m_pTrailTransform->Set_State(CTransform::STATE_LOOK, vRight * vTrailScale.x);
	m_pTrailTransform->Set_State(CTransform::STATE_RIGHT, VECTOR_UP * vTrailScale.z);
}

void CThorn::SetupScaleUpStart(_float fLength)
{
	m_bScaleUp = true;
	m_bRender = true;
	m_fLength = fLength;
}

void CThorn::SetupScaleUpDownStart(_float fLength)
{
	m_bScaleUpDown = true;
	m_bRender = true;
	m_fLength = fLength;
}

void CThorn::SetupScaleSmoothUpStart(_float fLength)
{
	m_bScaleSmoothUp = true;
	m_bRender = true;
	m_fLength = fLength;
}

void CThorn::SetupScaleSmoothDownStart()
{
	m_bScaleSmoothDown = true;
}


_bool CThorn::ScaleUpProcess(_double TimeDelta)
{
	_float vRightLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_RIGHT)));
	_float vUpLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_UP)));
	_float vLookLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_LOOK)));

	_float3 vLength;
	if (m_eThornType == THORN)
	{
		m_bFloorRender = true;
		m_bGardRender = true;
		m_fScaleAcc += TimeDelta;
		//vLength.x = vRightLength + powf(vRightLength, m_fScaleAcc);
		//vLength.y = vUpLength + powf(vUpLength, m_fScaleAcc);
		//vLength.z = vLookLength + powf(vLookLength, m_fScaleAcc);

		vLength.x = vRightLength + powf(m_fScaleAcc, 2.f) * 900.f;
		vLength.y = vUpLength + powf(m_fScaleAcc, 2.f) * 900.f;
		vLength.z = vLookLength + powf(m_fScaleAcc, 2.f) * 900.f;
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

_bool CThorn::ScaleUpSmoothProcess(_double TimeDelta)
{
	_float vRightLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_RIGHT)));
	_float vUpLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_UP)));
	_float vLookLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_LOOK)));

	_float3 vLength;
	if (m_eThornType == THORN)
	{
		m_bFloorRender = true;
		m_bGardRender = true;
		m_fScaleAcc += TimeDelta;
		//vLength.x = vRightLength + powf(vRightLength, m_fScaleAcc);
		//vLength.y = vUpLength + powf(vUpLength, m_fScaleAcc);
		//vLength.z = vLookLength + powf(vLookLength, m_fScaleAcc);

		vLength.x = vRightLength + powf(m_fScaleAcc, 2.f) * 3.f;
		vLength.y = vUpLength + powf(m_fScaleAcc, 2.f) * 3.f;
		vLength.z = vLookLength + powf(m_fScaleAcc, 2.f) * 3.f;
	}
	else if (m_eThornType == MISSILE)
	{
		vLength.x = vRightLength + TimeDelta * 5.f;
		vLength.y = vUpLength + TimeDelta * 2.f;
		vLength.z = vLookLength + TimeDelta * 5.f;
	}

	m_pTransform->Set_Scale(vLength);

	if (vLength.y >= m_fLength)
	{
		m_bScaleSmoothUp = false;
		m_bScaleFinish = true;
		return true;
	}
	else
		return false;
}

_bool CThorn::ScaleDownSmoothProcess(_double TimeDelta)
{
	_float vRightLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_RIGHT)));
	_float vUpLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_UP)));
	_float vLookLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_LOOK)));

	_float3 vLength;
	if (m_eThornType == THORN)
	{
		m_fScaleAcc += TimeDelta;
		//vLength.x = vRightLength + powf(vRightLength, m_fScaleAcc);
		//vLength.y = vUpLength + powf(vUpLength, m_fScaleAcc);
		//vLength.z = vLookLength + powf(vLookLength, m_fScaleAcc);

		vLength.x = vRightLength - powf(m_fScaleAcc, 2.f) * 3.f;
		vLength.y = vUpLength - powf(m_fScaleAcc, 2.f) * 3.f;
		vLength.z = vLookLength - powf(m_fScaleAcc, 2.f) * 3.f;
	}
	
	if (vLength.y <= m_fPrevScale.y)
	{
		m_bScaleSmoothDown = false;
		m_bScaleDownFinish = true;
		m_bScaleFinish = true;

		Reset();
		m_pTransform->Set_Scale(m_fPrevScale);
		return true;
	}
	else
	{
		m_pTransform->Set_Scale(vLength);
		return false;
	}
}

_bool CThorn::MoveProcess(_double TimeDelta)
{
	//가시의 Up방향으로 이동 MissileType일때
	_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
	_vector vUp = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_UP));

	m_fMoveAcc += TimeDelta;
	m_vTrailAcc += TimeDelta * 0.1f;
	vPos = vPos + vUp * powf(m_fMoveAcc, 0.8f) * 25.f * TimeDelta;

	_vector vTrailUp = m_pTrailTransform->Get_State(CTransform::STATE_UP);
	_vector vTrailDir = XMVector3Normalize(m_pTrailTransform->Get_State(CTransform::STATE_UP));
	if (XMVectorGetX(XMVector3Length(vTrailUp)) < 50.f)
	{
		vTrailUp = vTrailUp * (1.f + m_vTrailAcc);
		m_pTrailTransform->Set_State(CTransform::STATE_UP, vTrailUp);
	}

	_float fHalfLength = XMVectorGetX(XMVector3Length(m_pTransform->Get_State(CTransform::STATE_UP))) * 0.5f;
	_vector vTrailPos = vPos - vTrailDir * fHalfLength;
	m_pTrailTransform->Set_State(CTransform::STATE_POSITION, vTrailPos - vTrailDir * (XMVectorGetX(XMVector3Length(vTrailUp) * 0.5f)));
	m_pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	if (m_fMoveAcc >= 2.f)
	{
		Reset();
		m_fMoveAcc = 0.f;
		m_vTrailAcc = 0.f;
	}

	return true;
}

HRESULT CThorn::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transformtrail"), (CComponent**)&m_pTrailTransform)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.fRotationSpeed = XMConvertToRadians(30.f);
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform3"), (CComponent**)&m_pFloorGardTransform, &desc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"), TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"), TEXT("com_shader1"), (CComponent**)&m_pFloorShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_thorngard"), TEXT("com_mode_gard"), (CComponent**)&m_pFloorGard)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_BOSS, TEXT("proto_com_model_thorn"), TEXT("com_model"), (CComponent**)&m_pModel)))
		return E_FAIL;

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(0.5f, 0.5f, 0.5f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider"), (CComponent**)&m_pCollider, &collDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_trans_floorefeect"), (CComponent**)&m_pFloorEffectTransform, &desc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_thorndiffuse"), TEXT("com_texture_diffuse"), (CComponent**)&m_pFloorEffectDiffuse)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_thornmask"), TEXT("com_texture_mask"), (CComponent**)&m_pFloorEffectMask)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_gardmask"), TEXT("com_texture_gardmask"), (CComponent**)&m_pFloorGardTexture)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"), TEXT("com_buffer"), (CComponent**)&m_pFloorEffectBuffer)))
		return E_FAIL;

	////트레일
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_trail"), TEXT("com_shadertrail"), (CComponent**)&m_pTrailEffectShader)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"), TEXT("com_buffertrail"), (CComponent**)&m_pTrailEffectBuffer)))
		return E_FAIL;
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_trail"), TEXT("com_texture_masktrail"), (CComponent**)&m_pTrailTexture)))
		return E_FAIL;

	return S_OK;
}

HRESULT CThorn::SetupShaderResources()
{
	if (nullptr == m_pShader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

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

	Safe_Release(m_pCollider);
	Safe_Release(m_pRenderer);
	Safe_Release(m_pTransform);
	Safe_Release(m_pModel);
	Safe_Release(m_pShader);

	Safe_Release(m_pFloorEffectTransform);
	Safe_Release(m_pFloorEffectBuffer);
	Safe_Release(m_pFloorShader);

	Safe_Release(m_pFloorEffectDiffuse);
	Safe_Release(m_pFloorEffectMask);

	Safe_Release(m_pFloorGard);
	Safe_Release(m_pFloorGardTransform);
	Safe_Release(m_pFloorGardTexture);

	Safe_Release(m_pTrailEffectBuffer);
	Safe_Release(m_pTrailTransform);
	Safe_Release(m_pTrailTexture);
	Safe_Release(m_pTrailEffectShader);
}

void CThorn::SameObjectNoDetection()
{
}

void CThorn::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	CCharacter* pPlayer = dynamic_cast<CCharacter*>(dest->GetOwner());
	if (pPlayer)
	{
		CCollider* pPlayerColl = pPlayer->GetBodyCollider();
		if (dest->Compare(pPlayerColl))
		{
			switch (m_eThornType)
			{
			case THORN:
				pPlayer->RecvDamage(10.f);
				break;
			case MISSILE:
				//pPlayer->Airbone();
				pPlayer->Hit();
				pPlayer->RecvDamage(25.f);
				break;
			}
		}

	}
}

void CThorn::OnCollisionStay(CCollider * src, CCollider * dest)
{
}

void CThorn::OnCollisionExit(CCollider * src, CCollider * dest)
{
}
