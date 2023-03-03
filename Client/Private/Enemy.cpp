#include "pch.h"
#include "..\Public\Enemy.h"

#include "GameInstance.h"
#include "Bone.h"

CEnemy::CEnemy(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemy::CEnemy(const CEnemy & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemy::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEnemy::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	AddComponents();

	bone = model->GetBonePtr("Bip001");

	return S_OK;
}

void CEnemy::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(collider);

}

void CEnemy::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	CAnimation::ANIMATION_DESC animDesc;
	ZeroMemory(&animDesc, sizeof(CAnimation::ANIMATION_DESC));
	animDesc.Clip = animation;
	animDesc.Type = CAnimation::LOOP;

	model->Setup_Animation(animDesc.Clip, true);
	model->Play_Animation(TimeDelta, transform, animDesc.Type);


	_matrix tranMatrix = XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&transform->Get_WorldMatrix());
	collider->Update(tranMatrix);

	if (nullptr != renderer)
		renderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
	
}

HRESULT CEnemy::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = model->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		if (!strcmp("Cheek", model->GetMeshName(i)) || !strcmp("Cheek01", model->GetMeshName(i)))
			continue;

		model->Setup_ShaderMaterialResource(shader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);

		model->Setup_BoneMatrices(shader, "g_BoneMatrix", i);
		shader->Begin(0);
		model->Render(i);
	}

	return S_OK;
}

void CEnemy::RenderGUI()
{
}

_float4 CEnemy::GetPosition()
{
	if (transform == nullptr)
		return _float4();

	_float4 vPos;
	XMStoreFloat4(&vPos, transform->Get_State(CTransform::STATE_POSITION));

	return vPos;
}

HRESULT CEnemy::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&renderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&transform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"), TEXT("com_shader"), (CComponent**)&shader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_enemy01"), TEXT("com_model"), (CComponent**)&model)))
		return E_FAIL;

	transform->Set_State(CTransform::STATE_POSITION, XMVectorSet(25.f, 0.f, 40.f, 1.f));

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtants = _float3(0.5f, 0.5f, 0.5f);
	collDesc.vRotaion = _float3(0.f, 0.f, 0.f);

	(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collider"), (CComponent**)&collider, &collDesc));

	return S_OK;
}

HRESULT CEnemy::SetupShaderResources()
{
	if (nullptr == shader)
		return E_FAIL;

	CGameInstance* pInstance = CGameInstance::GetInstance();

	if (pInstance->Input_KeyState_Custom(DIK_PGUP) == KEY_STATE::TAP)
		animation++;

	if (pInstance->Input_KeyState_Custom(DIK_PGDN) == KEY_STATE::TAP)
		animation--;

	if (FAILED(transform->Setup_ShaderResource(shader, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(shader->SetMatrix("g_ViewMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_VIEW))))
		return E_FAIL;

	if (FAILED(shader->SetMatrix("g_ProjMatrix", &pInstance->Get_Transform_float4x4(CPipeLine::TS_PROJ))))
		return E_FAIL;

	if (FAILED(shader->SetRawValue("g_vCamPosition", &pInstance->Get_CamPosition(), sizeof(_float3))))
		return E_FAIL;

	const LIGHT_DESC* LightDesc = pInstance->GetLightDesc(0);
	if (nullptr == LightDesc)
		return E_FAIL;

	if (FAILED(shader->SetRawValue("g_vLightDir", &LightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(shader->SetRawValue("g_vLightDiffuse", &LightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(shader->SetRawValue("g_vLightSpecular", &LightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(shader->SetRawValue("g_vLightAmbient", &LightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CEnemy * CEnemy::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnemy*	pInstance = new CEnemy(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEnemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnemy::Clone(void * pArg)
{
	CEnemy* pInstance = new CEnemy(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEnemy");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemy::Free()
{
	__super::Free();

	Safe_Release(renderer);
	Safe_Release(transform);
	Safe_Release(model);
	Safe_Release(shader);
	Safe_Release(collider);
}
