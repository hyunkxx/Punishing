#include "pch.h"
#include "..\Public\Boss.h"

#include "ApplicationManager.h"

#include "PipeLine.h"
#include "GameInstance.h"

#include "Bone.h"
#include "Character.h"
#include "PlayerCamera.h"

CBoss::CBoss(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEnemy(pDevice, pContext)
{
}

CBoss::CBoss(const CBoss & rhs)
	: CEnemy(rhs)
{
}

HRESULT CBoss::Initialize_Prototype()
{
	ZeroMemory(&m_State, sizeof(ENEMY_STATE));
	m_State.fMaxHp = 20000.f;
	m_State.fCurHp = 20000.f;
	
	return S_OK;
}

HRESULT CBoss::Initialize(void * pArg)
{
	m_pAppManager = CApplicationManager::GetInstance();

	AddComponents();

	if (nullptr != pArg)
	{
		m_pPlayer = (CCharacter*)pArg;
		m_pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(L"com_transform"));
	}

	bone = model->GetBonePtr("Bip001");
	XMStoreFloat4x4(&m_RootBoneMatrix, XMLoadFloat4x4(&bone->GetOffSetMatrix()) * XMLoadFloat4x4(&bone->GetCombinedMatrix()) * XMLoadFloat4x4(&model->GetLocalMatrix()) * XMLoadFloat4x4(&transform->Get_WorldMatrix()));
	
	transform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 27.f, 1.f));
	transform->SetRotation(VECTOR_UP, XMConvertToRadians(180.f));

	return S_OK;
}

void CBoss::Tick(_double TimeDelta)
{
	TimeDelta = Freeze(TimeDelta);

	model->Setup_Animation(3, CAnimation::TYPE::ONE, true);

	m_bAlpha = false;


	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	pGameInstance->AddCollider(collider);
	pGameInstance->AddCollider(m_pOverlapCollider);
}

void CBoss::LateTick(_double TimeDelta)
{
	TimeDelta = Freeze(TimeDelta);
	
	model->Play_Animation(TimeDelta, transform, 0.1);
	collider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));
	m_pOverlapCollider->Update(XMLoadFloat4x4(&transform->Get_WorldMatrix()));

	if (nullptr != renderer)
		renderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CBoss::Render()
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

		if (m_bAlpha)
			shader->Begin(1);
		else
			shader->Begin(0);

		model->Render(i);
	}

	return S_OK;
}

void CBoss::RenderGUI()
{
}

HRESULT CBoss::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&renderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 1.5f;
	TransformDesc.fRotationSpeed = 5.f;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&transform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"), TEXT("com_shader"), (CComponent**)&shader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_model_boss"), TEXT("com_model"), (CComponent**)&model)))
		return E_FAIL;

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.f, 0.f);
	collDesc.vExtents = _float3(1.5f, 2.f, 1.5f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider"), (CComponent**)&collider, &collDesc);

	ZeroMemory(&collDesc, sizeof collDesc);
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.3f, 0.f);
	collDesc.vExtents = _float3(1.1f, 1.1f, 1.1f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_sphere_collider"), TEXT("com_collder_enemy"), (CComponent**)&m_pOverlapCollider, &collDesc);

	return S_OK;
}

HRESULT CBoss::SetupShaderResources()
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

_float4 CBoss::GetPosition()
{
	if (transform == nullptr)
		return _float4();

	if (IsDestroy())
		return _float4();

	_float4 vPos;
	XMStoreFloat4(&vPos, transform->Get_State(CTransform::STATE_POSITION));

	return vPos;
}

_float CBoss::GetLengthFromCamera()
{
	CPipeLine* pPipeLine = CPipeLine::GetInstance();

	_vector vPos = transform->Get_State(CTransform::STATE_POSITION);
	_vector vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	_float fLength = XMVectorGetX(XMVector3Length(vPos - vCamPos));

	return fLength;
}

void CBoss::OnCollisionEnter(CCollider * src, CCollider * dest)
{
}

void CBoss::OnCollisionStay(CCollider * src, CCollider * dest)
{
	CPlayerCamera* pCamera = dynamic_cast<CPlayerCamera*>(dest->GetOwner());
	if (src->Compare(m_pOverlapCollider) && pCamera && dest->Compare(m_pCamera->GetCollider()))
	{
		m_bAlpha = true;
	}
}

void CBoss::OnCollisionExit(CCollider * src, CCollider * dest)
{

}

CBoss * CBoss::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CBoss*	pInstance = new CBoss(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CBoss::Clone(void * pArg)
{
	CBoss* pInstance = new CBoss(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CBoss");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBoss::Free()
{
	__super::Free();
}
