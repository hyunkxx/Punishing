#include "pch.h"
#include "..\Public\Wall.h"

#include "GameInstance.h"

CWall::CWall(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWall::CWall(const CGameObject & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWall::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CWall::Initialize(void * pArg)
{
	memcpy(&m_CubeDesc, (CUBE_DESC*)pArg, sizeof(CUBE_DESC));
	if (m_CubeDesc.vScale.x == 110.f && m_CubeDesc.vScale.y == 1.f && m_CubeDesc.vScale.z == 110.f)
	{
		//보스룸 세팅
		if (FAILED(AddComponents()))
			return E_FAIL;
	}
	else
	{
		//게임플레이 세팅
		if (FAILED(AddComponents_GamePlay()))
			return E_FAIL;
	}

	ObjectID = 1;

	if (pArg != nullptr)
	{
		memcpy(&m_CubeDesc, (CUBE_DESC*)pArg, sizeof(CUBE_DESC));
		m_pTransform->Set_Scale(m_CubeDesc.vScale);
		m_pTransform->SetRotationXYZ(m_CubeDesc.vRotation);
		m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CubeDesc.vPosition));
	}
	else
	{
		m_CubeDesc.vScale = m_pTransform->Get_Scale();
		XMStoreFloat3(&m_CubeDesc.vPosition, m_pTransform->Get_State(CTransform::STATE_POSITION));
	}

	XMStoreFloat3(&m_vPlanePos[LEFT], m_pTransform->Get_State(CTransform::STATE_POSITION) - m_pTransform->Get_State(CTransform::STATE_RIGHT) * 0.5f);
	XMStoreFloat3(&m_vPlanePos[RIGHT], m_pTransform->Get_State(CTransform::STATE_POSITION) + m_pTransform->Get_State(CTransform::STATE_RIGHT) * 0.5f);
	XMStoreFloat3(&m_vPlanePos[FRONT], m_pTransform->Get_State(CTransform::STATE_POSITION) + m_pTransform->Get_State(CTransform::STATE_LOOK) * 0.5f);
	XMStoreFloat3(&m_vPlanePos[BACK], m_pTransform->Get_State(CTransform::STATE_POSITION) - m_pTransform->Get_State(CTransform::STATE_LOOK) * 0.5f);

	m_vPlanePos[LEFT].y = 0.0f;
	m_vPlanePos[RIGHT].y = 0.0f;
	m_vPlanePos[FRONT].y = 0.0f;
	m_vPlanePos[BACK].y = 0.0f;

	m_pTransform->Set_Scale(m_CubeDesc.vScale);
	m_pTransform->SetRotationXYZ(m_CubeDesc.vRotation);
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CubeDesc.vPosition));

	m_pCollider->SetVisible(false);

	return S_OK;
}

void CWall::Tick(_double TimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	pGameInstance->AddCollider(m_pCollider, 1);
	m_pCollider->Update(XMLoadFloat4x4(&m_pTransform->Get_WorldMatrix()));

}

void CWall::LateTick(_double TimeDelta)
{
}

HRESULT CWall::Render()
{
	return S_OK;
}

void CWall::RenderGUI()
{
}

_float CWall::GetDepth()
{
	if (nullptr != m_pTransform)
		return m_CubeDesc.vScale.x * 0.5f;
	else
		return 0.0f;
}

_vector CWall::GetPosition()
{
	if (nullptr != m_pTransform)
		return m_pTransform->Get_State(CTransform::STATE_POSITION);
	else
		return POSITION_ZERO;
}

_vector CWall::GetNormal(_fvector vPosition, _float* vLength_out)
{
	_vector vNormal;

	_float fNear = FLT_MAX;
	for (_uint i = 0; i < 4; ++i)
	{
		_vector vDistance = XMLoadFloat3(&m_vPlanePos[i]) - vPosition;
		_float vLength = XMVectorGetX(XMVector3Length(vDistance));

		if (vLength < fNear)
		{
			fNear = vLength;
			switch (i)
			{
			case LEFT:
				vNormal = -m_pTransform->Get_State(CTransform::STATE_RIGHT);
				break;
			case RIGHT:
				vNormal = m_pTransform->Get_State(CTransform::STATE_RIGHT);
				break;
			case FRONT:
				vNormal = m_pTransform->Get_State(CTransform::STATE_LOOK);
				break;
			case BACK:
				vNormal = -m_pTransform->Get_State(CTransform::STATE_LOOK);
				break;
			}
		}
	}

	//Y값 0으로 세팅;
	*vLength_out = fNear;
	vNormal = XMVectorSetY(vNormal, 0.f);
	return XMVector3Normalize(vNormal);
}

void CWall::SetScale(_float3 vScale)
{
	m_CubeDesc.vScale = vScale;
	m_pTransform->Set_Scale(m_CubeDesc.vScale);
}

void CWall::SetRotation(_float3 vRotation)
{
	m_CubeDesc.vRotation = vRotation;
	m_pTransform->SetRotationXYZ(m_CubeDesc.vRotation);
}

void CWall::SetPosition(_float3 vPosition)
{
	m_CubeDesc.vPosition = vPosition;
	m_pTransform->Set_State(CTransform::STATE_POSITION, XMLoadFloat3(&m_CubeDesc.vPosition));
}


HRESULT CWall::AddComponents()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 0.0f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(0.0f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(31.5f, 0.f, 19.5f, 1.f));

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(1.f, 1.f, 1.f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_BOSS, TEXT("proto_com_obb_collider"), TEXT("com_collider"), (CComponent**)&m_pCollider, &collDesc)))
		return E_FAIL;

	m_pCollider->SetVisible(true);
	m_pCollider->SetActive(true);

	return S_OK;
}

HRESULT CWall::AddComponents_GamePlay()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	CTransform::TRANSFORM_DESC TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORM_DESC));

	TransformDesc.fMoveSpeed = 0.0f;
	TransformDesc.fRotationSpeed = XMConvertToRadians(0.0f);

	if (FAILED(CGameObject::Add_Component(LEVEL_STATIC, TEXT("proto_com_transform"), TEXT("com_transform"), (CComponent**)&m_pTransform, &TransformDesc)))
		return E_FAIL;

	m_pTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(31.5f, 0.f, 19.5f, 1.f));

	CCollider::COLLIDER_DESC collDesc;
	collDesc.owner = this;
	collDesc.vCenter = _float3(0.f, 0.f, 0.f);
	collDesc.vExtents = _float3(1.f, 1.f, 1.f);
	collDesc.vRotation = _float3(0.f, 0.f, 0.f);

	if (FAILED(CGameObject::Add_Component(LEVEL_GAMEPLAY, TEXT("proto_com_obb_collider"), TEXT("com_collider"), (CComponent**)&m_pCollider, &collDesc)))
		return E_FAIL;

	m_pCollider->SetVisible(true);
	m_pCollider->SetActive(true);

	return S_OK;
}

CWall * CWall::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWall*	pInstance = new CWall(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Create : CWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CGameObject * CWall::Clone(void * pArg)
{
	CGameObject* pInstance = new CWall(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CWall");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWall::Free()
{
	__super::Free();

	Safe_Release(m_pCollider);
	Safe_Release(m_pTransform);
}
