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

CPipeLine::RAY_DESC CPipeLine::CreateWorldRay(const CLIENT_DESC & clientDesc, _float rayDistance)
{
	RAY_DESC rayDesc;
	ZeroMemory(&rayDesc, sizeof(rayDesc));
	rayDesc.mRayDistance = rayDistance;
	rayDesc.mHitDistance = rayDistance;

	_matrix matViewInverse = Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_matrix matProjInverse = Get_Transform_Matrix_Inverse(CPipeLine::TS_PROJ);

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(clientDesc.hWnd, &mousePos);

	_float3 vMouse;
	vMouse.x = mousePos.x /  (clientDesc.mViewportSize.x * 0.5f) - 1.f;
	vMouse.y = mousePos.y / -(clientDesc.mViewportSize.y * 0.5f) + 1.f;
	vMouse.z = 0.f;

	_vector vMousePos = XMVector3TransformCoord(XMLoadFloat3(&vMouse), matProjInverse);

	//View-Space Origin
	_vector vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_vector vRayDir = vMousePos - vRayPos;

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	vRayDir = XMVector3Normalize(vRayDir);

	XMStoreFloat3(&rayDesc.mRayPos, vRayPos);
	XMStoreFloat3(&rayDesc.mRayDir, vRayDir);

	return rayDesc;
}

CPipeLine::RAY_DESC CPipeLine::CreateLocalRay(const CLIENT_DESC & clientDesc, _matrix matWorldInverse, _float rayDistance)
{
	RAY_DESC rayDesc;
	ZeroMemory(&rayDesc, sizeof(rayDesc));
	rayDesc.mRayDistance = rayDistance;

	_matrix matViewInverse = Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);
	_matrix matProjInverse = Get_Transform_Matrix_Inverse(CPipeLine::TS_PROJ);

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(clientDesc.hWnd, &mousePos);

	_float3 vMouse;
	vMouse.x = mousePos.x / (clientDesc.mViewportSize.x * 0.5f) - 1.f;
	vMouse.y = mousePos.y / -(clientDesc.mViewportSize.y * 0.5f) + 1.f;
	vMouse.z = 0.f;

	_vector vMousePos = XMVector3TransformCoord(XMLoadFloat3(&vMouse), matProjInverse);

	//View-Space Origin
	_vector vRayPos = { 0.f, 0.f, 0.f, 1.f };
	_vector vRayDir = vMousePos - vRayPos;

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	vRayPos = XMVector3TransformCoord(vRayPos, matWorldInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matWorldInverse);

	vRayDir = XMVector3Normalize(vRayDir);

	XMStoreFloat3(&rayDesc.mRayPos, vRayPos);
	XMStoreFloat3(&rayDesc.mRayDir, vRayDir);

	return rayDesc;
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