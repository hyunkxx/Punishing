#include "pch.h"
#include "..\Public\Weapon.h"

#include "GameInstance.h"
#include "Character.h"
#include "Bone.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

HRESULT CWeapon::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;
	
	if (nullptr == pArg)
		return E_FAIL;

	m_descOwner = *(OWNER_DESC*)pArg;



	return S_OK;
}

void CWeapon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CWeapon::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	_float4x4 playerWorldMatrix = m_descOwner.pTransform->Get_WorldMatrix();
	
	assert(m_descOwner.pModel);
	assert(m_descOwner.pTransform);
	assert(m_descOwner.pWeaponCase);

	//무기 본 위치
	_float4x4 WeaponBoneMatrix;
	XMStoreFloat4x4(&WeaponBoneMatrix, 
		XMLoadFloat4x4(&m_descOwner.pWeaponCase->GetOffSetMatrix()) * XMLoadFloat4x4(&m_descOwner.pWeaponCase->GetCombinedMatrix())
		* XMLoadFloat4x4(&m_descOwner.pModel->GetLocalMatrix()) * XMLoadFloat4x4(&playerWorldMatrix));

	mTransform->Set_WorldMatrix(WeaponBoneMatrix);

	if (nullptr != mRenderer)
		mRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CWeapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_float4 vGlowColor = { 0.f, 1.f, 0.f, 1.f };
	mShader->SetRawValue("g_GlowColor", &vGlowColor, sizeof(_float4));
	_uint MeshCount = mModel->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		mModel->Setup_ShaderMaterialResource(mShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
		
		mShader->Begin(12);
		mModel->Render(i);
	}

	return S_OK;
}

void CWeapon::RenderGUI()
{
}

HRESULT CWeapon::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&mRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&mTransform)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"), TEXT("com_shader"), (CComponent**)&mShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_kamui_weapon"), TEXT("com_model"), (CComponent**)&mModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::SetupShaderResources()
{
	if (nullptr == mShader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (FAILED(mTransform->Setup_ShaderResource(mShader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(mShader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(mShader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(mShader->SetRawValue("g_vCamPosition", &pInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	const LIGHT_DESC* LightDesc = pInstance->GetLightDesc(0);
	if (nullptr == LightDesc)
		return E_FAIL;

	if (FAILED(mShader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(mShader->SetRawValue("g_vLightDiffuse", &LightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(mShader->SetRawValue("g_vLightSpecular", &LightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(mShader->SetRawValue("g_vLightAmbient", &LightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	return S_OK;

}

CWeapon* CWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon* pInstance = new CWeapon(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CWeapon::Clone(void * pArg)
{
	CWeapon* pInstance = new CWeapon(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWeapon");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(mRenderer);
	Safe_Release(mTransform);
	Safe_Release(mModel);
	Safe_Release(mShader);

}
