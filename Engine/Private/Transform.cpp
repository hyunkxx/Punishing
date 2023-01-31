#include "..\Public\Transform.h"
#include "Shader.h"

CTransform::CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTransform::CTransform(const CTransform& rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

_float3 CTransform::Get_Scale()
{
	_vector vRight = Get_State(STATE::STATE_RIGHT);
	_vector vUp = Get_State(STATE::STATE_UP);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	return _float3(
		XMVectorGetX(XMVector3Length(vRight)),
		XMVectorGetX(XMVector3Length(vUp)),
		XMVectorGetX(XMVector3Length(vLook)));
}

void CTransform::Set_State(STATE eState, _fvector vState)
{
	_float4 vStateFloat4;
	XMStoreFloat4(&vStateFloat4, vState);

	memcpy(((_float4*)&m_WorldMatrix + eState), &vStateFloat4, sizeof(_float3));
}

HRESULT CTransform::Initialize_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	ZeroMemory(&m_TransformDesc, sizeof TRANSFORM_DESC);

	return S_OK;
}

HRESULT CTransform::Initialize(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORM_DESC));

	return S_OK;
}

void CTransform::MoveForward(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::MoveBackward(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vLook = Get_State(STATE::STATE_LOOK);

	vPosition += -XMVector3Normalize(vLook) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::MoveRight(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vRight = Get_State(STATE::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::MoveLeft(_double TimeDelta)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vRight = Get_State(STATE::STATE_RIGHT);

	vPosition += -XMVector3Normalize(vRight) * m_TransformDesc.fMoveSpeed * (_float)TimeDelta;
	Set_State(STATE::STATE_POSITION, vPosition);
}

void CTransform::SetRotation(_fvector vAxis, _float fAngle)
{
	_vector vRight, vUp, vLook;
	_float3 vScale = Get_Scale();

	vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	_matrix RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, fAngle);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::STATE_RIGHT, vRight);
	Set_State(STATE::STATE_UP, vUp);
	Set_State(STATE::STATE_LOOK, vLook);
}

void CTransform::Rotate(_fvector vAxis, _double TimeDelta)
{
	_vector vRight, vUp, vLook;
	_float3 vScale = Get_Scale();

	vRight = Get_State(STATE::STATE_RIGHT);
	vUp = Get_State(STATE::STATE_UP);
	vLook = Get_State(STATE::STATE_LOOK);

	_matrix RotationMatrix;
	RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationSpeed * (_float)TimeDelta);

	vRight = XMVector3TransformNormal(vRight, RotationMatrix);
	vUp = XMVector3TransformNormal(vUp, RotationMatrix);
	vLook = XMVector3TransformNormal(vLook, RotationMatrix);

	Set_State(STATE::STATE_RIGHT, vRight);
	Set_State(STATE::STATE_UP, vUp);
	Set_State(STATE::STATE_LOOK, vLook);
}

void CTransform::LookAt(_fvector vTargetPos)
{
	_vector vPosition = Get_State(STATE::STATE_POSITION);
	_vector vLook = vTargetPos - vPosition;

	_vector vRight = XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook);
	_vector vUp = XMVector3Cross(vLook, vRight);

	_float3 vScale = Get_Scale();

	Set_State(STATE::STATE_RIGHT, XMVector3Normalize(vRight) * vScale.x);
	Set_State(STATE::STATE_UP, XMVector3Normalize(vUp) * vScale.y);
	Set_State(STATE::STATE_LOOK, XMVector3Normalize(vLook) * vScale.z);
}

_float CTransform::Compute_Distance(_fvector vTargetPos)
{
	return XMVectorGetX(XMVector3Length(vTargetPos - Get_State(STATE::STATE_POSITION)));
}

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimitDistance)
{
	LookAt(vTargetPos);

	if (fLimitDistance <= Compute_Distance(vTargetPos))
		MoveForward(TimeDelta);
}

HRESULT CTransform::Setup_ShaderResource(CShader* pShaderCom, const char* pConstantName)
{
	return pShaderCom->SetMatrix(pConstantName, &m_WorldMatrix);
}

CTransform* CTransform::Create(ID3D11Device* pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
