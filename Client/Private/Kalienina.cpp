#include "pch.h"
#include "..\Public\Kalienina.h"
#include "GameInstance.h"
#include "Bone.h"

CKalienina::CKalienina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
{
}

CKalienina::CKalienina(const CKalienina & rhs)
	: CGameObject(rhs)
{
}

HRESULT CKalienina::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CKalienina::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(AddComponents()))
		return E_FAIL;

	mModel->Setup_Animation(eCurrentClip);

	return S_OK;
}

void CKalienina::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	__super::Tick(TimeDelta);

	AnimationControl();
	Movement(TimeDelta);

	mTransform->MoveForward(TimeDelta);
}

void CKalienina::LateTick(_double TimeDelta)
{
	__super::LateTick(TimeDelta);

	//CBone* pRootBone = mModel->GetBonePtr("R3KalieninaMd010031");

	//_float4x4 BipMatrix = pBip->GetCombinedMatrix();
	//vLeftToePos = XMVectorSet(BipMatrix._41, BipMatrix._42, BipMatrix._43, BipMatrix._44);
	//_float4x4 vWorldMatrix = mTransform->Get_WorldMatrix();
	
	//mTransform->Set_State(CTransform::STATE_POSITION, );
	//pRootBone->SetTransformationMatrix(XMLoadFloat4x4(&vWorldMatrix));

	mModel->Play_Animation(TimeDelta);
	
	if (nullptr != mRenderer)
		mRenderer->Add_RenderGroup(CRenderer::RENDER_NONALPHA, this);
}

HRESULT CKalienina::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetupShaderResources()))
		return E_FAIL;

	_uint MeshCount = mModel->Get_MeshCount();
	for (_uint i = 0; i < MeshCount; ++i)
	{
		mModel->Setup_ShaderMaterialResource(mShader, "g_DiffuseTexture", i, aiTextureType::aiTextureType_DIFFUSE);
		//m_pModelCom->SetUp_ShaderMaterialResource(m_pShaderCom, "g_AmbientTexture", i, aiTextureType_AMBIENT);

		mModel->Setup_BoneMatrices(mShader, "g_BoneMatrix", i);
		mShader->Begin(0);
		mModel->Render(i);
	}

	return S_OK;
}

void CKalienina::RenderGUI()
{
	const CBone* pRootBone = mModel->GetBonePtr("R3KalieninaMd010031");
	_float4x4 RootMatrix = pRootBone->GetCombinedMatrix();
	_vector vPosition = XMVectorSet(RootMatrix._41, RootMatrix._42, RootMatrix._43, RootMatrix._44);
	mTransform->Set_State(CTransform::STATE_POSITION, vPosition);

	ImGui::Begin("Transform");
	_float3 vPos;
	XMStoreFloat3(&vPos, mTransform->Get_State(CTransform::STATE_POSITION));

	float a[3];
	_float3 vp;
	//XMStoreFloat3(a, vLeftToePos);
	a[0] = vPos.x;
	a[1] = vPos.y;
	a[2] = vPos.z;
	ImGui::InputFloat3("Position", a);

	ImGui::End();
}

const CBone * CKalienina::GetBone(const char * szBoneName) const
{
	return mModel->GetBonePtr(szBoneName);
}

HRESULT CKalienina::AddComponents()
{
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"), TEXT("com_renderer"), (CComponent**)&mRenderer)))
		return E_FAIL;

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 5.f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&mTransform, &TransformDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_shader_vtxanimmodel"), TEXT("com_shader"), (CComponent**)&mShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_model_kalienina"), TEXT("com_model"), (CComponent**)&mModel)))
		return E_FAIL;

	return S_OK;
}

HRESULT CKalienina::SetupShaderResources()
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

void CKalienina::Movement(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_UPARROW) == KEY_STATE::HOLD)
	{
		eCurrentClip = CLIP::RUN;
		mTransform->MoveForward(TimeDelta);
	}
}

void CKalienina::AnimationControl()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (pGameInstance->Input_KeyState_Custom(DIK_Z) == KEY_STATE::TAP)
	{
		eCurrentClip = CLIP::ATTACK1;
	}

	if (mModel->AnimationIsFinish())
	{
		eCurrentClip = CLIP::STAND1;
	}
	
	mModel->Setup_Animation(eCurrentClip);
}

CKalienina* CKalienina::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CKalienina*	pInstance = new CKalienina(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CKalienina");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject* CKalienina::Clone(void* pArg)
{
	CKalienina* pInstance = new CKalienina(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CKalienina");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CKalienina::Free()
{
	__super::Free();

	Safe_Release(mRenderer);
	Safe_Release(mTransform);
	Safe_Release(mModel);
	Safe_Release(mShader);
}
