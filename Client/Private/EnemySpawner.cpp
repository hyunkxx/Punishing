#include "pch.h"
#include "..\Public\EnemySpawner.h"

#include "GameInstance.h"
#include "ApplicationManager.h"
#include "Layer.h"
#include "Enemy.h"
#include "Character.h"
#include "PlayerCamera.h"

CEnemySpawner::CEnemySpawner(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CEnemySpawner::CEnemySpawner(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CEnemySpawner::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CEnemySpawner::Initialize(void * pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	if (FAILED(AddComponents()))
		return E_FAIL;

	m_pCollider->SetVisible(true);

	//CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("layer_camera"));
	//if (nullptr == (m_pPlayerCamera = static_cast<CPlayerCamera*>(pLayer->Find_GameObject(L"player_camera"))))
	//	return E_FAIL;

	return S_OK;
}

void CEnemySpawner::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	m_fDisAcc += TimeDelta * 0.5f;

	if (pGameInstance->Input_KeyState_Custom(DIK_9) == KEY_STATE::TAP)
	{
		//스포너 리셋임
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(31.f, 0.f, 60.f, 1.f));
		m_iRespawnCount = 0.f;
	}

	if (pGameInstance->Input_KeyState_Custom(DIK_0) == KEY_STATE::TAP)
	{
		CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("layer_enemy"));
		for (auto iter = pLayer->m_GameObjects.begin(); iter != pLayer->m_GameObjects.end(); ++iter)
		{
			_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
			_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK));
			_vector vSpawnPos = vPos + vLook * 8.f;

			static_cast<CEnemy*>(iter->second)->Reset(_float3(XMVectorGetX(vSpawnPos), 0.f, XMVectorGetZ(vSpawnPos)), _float(8.f));
			static_cast<CEnemy*>(iter->second)->LookPlayer();
			//static_cast<CCharacter*>(mPlayer)->ClearEnemyCheckCollider();
		}
	}

	if (m_isEnter)
	{
		int iActiveEnemyCount = 0;
		int iDisableeEnemyCount = 0;
		CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("layer_enemy"));

		_int iDeadWaitCount = 0;
		static bool bEmptySound = false;
		for (auto iter = pLayer->m_GameObjects.begin(); iter != pLayer->m_GameObjects.end(); ++iter)
		{
			if ((iter->second)->IsActive())
				iActiveEnemyCount++;

			else  if ((iter->second)->IsDisable())
				iDisableeEnemyCount++;

			if (!(iter->second)->IsDisable() && static_cast<CEnemy*>(iter->second)->IsDeadWait())
				iDeadWaitCount++;
		}

		if (m_iCurSpawnerIndex == SECOND_SPAWNER)
		{
			if (iDisableeEnemyCount + iDeadWaitCount == 5)
			{
				if (!bEmptySound)
				{
					bEmptySound = true;
					pGameInstance->PlaySoundEx(L"Empty.wav", SOUND_CHANNEL::WARNING, SOUND_VOLUME::CUSTOM_VOLUM, 0.2f);
				}
			}

			if (bEmptySound)
			{
				m_fBGMVolum -= 0.02 * TimeDelta;
				pGameInstance->SetSoundVolume(SOUND_CHANNEL::SOUND_BGM, SOUND_VOLUME::CUSTOM_VOLUM, m_fBGMVolum);

				if (m_fBGMVolum <= 0.f)
				{
					pGameInstance->StopSound(SOUND_CHANNEL::SOUND_BGM);
					m_pPlayerCamera->LevelEnd();
					Destroy();
				}
			}
		}


		if (iActiveEnemyCount == 0)
		{
			switch (m_iCurSpawnerIndex)
			{
			case FIRST_SPAWNER:
				m_iRespawnCount++;
				for (auto iter = pLayer->m_GameObjects.begin(); iter != pLayer->m_GameObjects.end(); ++iter)
				{
					_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
					_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK));
					_vector vSpawnPos = vPos + vLook * 8.f;

					static_cast<CEnemy*>(iter->second)->Reset(_float3(XMVectorGetX(vSpawnPos), 0.f, XMVectorGetZ(vSpawnPos)), _float(8.f));
					static_cast<CEnemy*>(iter->second)->LookPlayer();
				}

				if (m_iRespawnCount >= 0)
				{
					m_isEnter = false;
					m_iRespawnCount = 0;
					m_iCurSpawnerIndex = SECOND_SPAWNER;

					m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(28.f, 0.f, 60.f, 1.f));
					m_pTransform->SetRotation(VECTOR_UP, XMConvertToRadians(40.f));
				}
				break;
			case SECOND_SPAWNER:
			{
				break;
			}
			default:
				break;
			}
			
		}

	}
}

void CEnemySpawner::LateTick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pCollider);
	m_pCollider->Update(XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix()));

	m_pRenderer->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEnemySpawner::Render()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//if (FAILED(m_pShader->SetMatrix("g_WorldMatrix", &m_pWarpTransform->Get_WorldMatrix())))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetMatrix("g_ViewMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TRANSFORM_STATE::TS_VIEW))))
	//	return E_FAIL;
	//if (FAILED(m_pShader->SetMatrix("g_ProjMatrix", &pGameInstance->Get_Transform_float4x4(CPipeLine::TRANSFORM_STATE::TS_PROJ))))
	//	return E_FAIL;

	//if (FAILED(m_pWarpNoise->Setup_ShaderResource(m_pShader, "g_Texture")))
	//	return E_FAIL;

	//m_pShader->SetRawValue("g_TimeAcc", &m_fDisAcc, sizeof(float));
	//m_pShader->Begin(10);
	//m_pWarpBuffer->Render();

	return S_OK;
}

void CEnemySpawner::RenderGUI()
{
}

void CEnemySpawner::SetupSpawner(_float3 vPos, _float fRadius)
{
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&vPos));
}

HRESULT CEnemySpawner::AddComponents()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));
	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(28.f, 0.f, 34.f, 1.f));

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform2"), (CComponent**)&m_pWarpTransform, &TransformDesc)))
		return E_FAIL;
	m_pWarpTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(28.f, 5.f, 5.f, 1.f));
	m_pWarpTransform->Set_Scale(_float3(28.f, 4.f, 10.f));

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 1.f, 0.f);
	collDesc.vExtents = _float3(30.f, 2.f, 1.f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider"), (CComponent**)&m_pCollider, &collDesc)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_renderer"),
		TEXT("com_renderer"), (CComponent**)&m_pRenderer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		TEXT("com_vibuffer"), (CComponent**)&m_pWarpBuffer)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"),
		TEXT("com_shader"), (CComponent**)&m_pShader)))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_texture_dismask"),
		TEXT("com_noise"), (CComponent**)&m_pWarpNoise)))
		return E_FAIL;

	return S_OK;
}

CEnemySpawner * CEnemySpawner::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEnemySpawner*	pInstance = new CEnemySpawner(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CEnemySpawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CEnemySpawner::Clone(void * pArg)
{
	CGameObject* pInstance = new CEnemySpawner(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CEnemySpawner");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemySpawner::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
}

void CEnemySpawner::OnCollisionEnter(CCollider * src, CCollider * dest)
{
	if (m_isEnter)
		return;

	//처음 진입시 생성
	CCharacter* pPlayer = dynamic_cast<CCharacter*>(dest->GetOwner());
	if (pPlayer)
	{
		CCollider* pPlayerBodyCollider = pPlayer->GetBodyCollider();
		if (dest->Compare(pPlayerBodyCollider))
		{
			m_isEnter = true;

			//if (m_iCurSpawnerIndex == SECOND_SPAWNER)
			//{
			//	m_pWarpTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(0.f, 0.f, 0.f, 1.f));
			//}
			//else
			//{
			//	m_pWarpTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(28.f, 0.f, 60.f, 1.f));
			//	m_pWarpTransform->SetRotation(VECTOR_UP, XMConvertToRadians(40.f));
			//}

			CApplicationManager::GetInstance()->SetSpawned(true);
			CGameInstance* pGameInstance = CGameInstance::GetInstance();

			CLayer* pLayer = pGameInstance->Find_Layer(LEVEL_GAMEPLAY, TEXT("layer_enemy"));
			for (auto iter = pLayer->m_GameObjects.begin(); iter != pLayer->m_GameObjects.end(); ++iter)
			{
				_vector vPos = m_pTransform->Get_State(CTransform::STATE_POSITION);
				_vector vLook = XMVector3Normalize(m_pTransform->Get_State(CTransform::STATE_LOOK));
				_vector vSpawnPos;
				if(m_iCurSpawnerIndex == FIRST_SPAWNER)
					vSpawnPos = vPos + vLook * 6.f;
				else
					vSpawnPos = vPos + vLook * 13.f;

				static_cast<CEnemy*>(iter->second)->Reset(_float3(XMVectorGetX(vSpawnPos), 0.f, XMVectorGetZ(vSpawnPos)), _float(8.f));
				static_cast<CEnemy*>(iter->second)->LookPlayer();
			}

			//static_cast<CCharacter*>(pPlayer)->ClearEnemyCheckCollider();
		}
	}
}
