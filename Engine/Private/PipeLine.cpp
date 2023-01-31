#include "..\Public\PipeLine.h"

IMPLEMENT_SINGLETON(CPipeLine)

_float4x4 CPipeLine::Get_Transform_float4x4(TRANSFORM_STATE eState)
{
	return m_TransformMatrix[eState];
}

_float4x4 CPipeLine::Get_Transform_float4x4_Inverse(TRANSFORM_STATE eState)
{
	return m_TransformMatrixInverse[eState];
}

_matrix CPipeLine::Get_Transform_Matrix(TRANSFORM_STATE eState)
{
	return XMLoadFloat4x4(&m_TransformMatrix[eState]);
}

_matrix CPipeLine::Get_Transform_Matrix_Inverse(TRANSFORM_STATE eState)
{
	return XMLoadFloat4x4(&m_TransformMatrixInverse[eState]);
}

_float4 CPipeLine::Get_CamPosition()
{
	return m_vCamPosition;
}

void CPipeLine::Set_Transform(TRANSFORM_STATE eState, _fmatrix TransformMatrix)
{
	XMStoreFloat4x4(&m_TransformMatrix[eState], TransformMatrix);
}

HRESULT CPipeLine::Initialize()
{
	for (_uint i = 0; i < TS_END; ++i)
	{
		XMStoreFloat4x4(&m_TransformMatrix[i], XMMatrixIdentity());
		XMStoreFloat4x4(&m_TransformMatrixInverse[i], XMMatrixIdentity());
	}

	m_vCamPosition = _float4(0.f, 0.f, 0.f, 1.f);

	return S_OK;
}

void CPipeLine::Tick()
{
	for (_uint i = 0; i < TS_END; ++i)
	{
		XMStoreFloat4x4(&m_TransformMatrixInverse[i],
			XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_TransformMatrix[i])));
	}

	memcpy(&m_vCamPosition, &m_TransformMatrixInverse[TS_VIEW].m[3][0], sizeof(_float4));
}

void CPipeLine::Free()
{
}