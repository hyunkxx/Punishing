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

_bool CPipeLine::RaycastFromCusor(_vector v1, _vector v2, _vector v3, RAY_DESC* rayDesc, const CLIENT_DESC& clientDesc)
{
	//bool Intersects(FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, GXMVECTOR V1, HXMVECTOR V2, _Out_ float& Dist);
	_float4 matView4x4;
	_float4x4 matProj = Get_Transform_float4x4(CPipeLine::TS_PROJ);
	memcpy(&matView4x4, &(Get_Transform_float4x4_Inverse(CPipeLine::TS_VIEW).m[3][0]), sizeof(_float4));

	_matrix matView = Get_Transform_Matrix(CPipeLine::TS_VIEW);
	_matrix matProjInverse = Get_Transform_Matrix_Inverse(CPipeLine::TS_PROJ);
	_matrix matViewInverse = Get_Transform_Matrix_Inverse(CPipeLine::TS_VIEW);

	POINT mousePos;
	GetCursorPos(&mousePos);
	ScreenToClient(clientDesc.hWnd, &mousePos);

	_float3 vMouse;
	vMouse.x = mousePos.x / (clientDesc.mViewportSize.x * 0.5f) - 1.f;
	vMouse.y = mousePos.y / -(clientDesc.mViewportSize.y * 0.5f) + 1.f;
	vMouse.z = 0.f;

	_vector vMousePos = XMVector3TransformCoord(XMLoadFloat3(&vMouse), matProjInverse);

	//View-Space Origin
	_vector vRayPos = { 0.f, 0.f, 0.f };
	_vector vRayDir = XMVector3Normalize(vMousePos - vRayPos);

	vRayPos = XMVector3TransformCoord(vRayPos, matViewInverse);
	vRayDir = XMVector3TransformNormal(vRayDir, matViewInverse);

	XMStoreFloat3(&rayDesc->mRayPos, vRayPos);
	XMStoreFloat3(&rayDesc->mRayDir, vRayDir);
	
	rayDesc->mHitDistance = rayDesc->mRayDistance;
	if (TriangleTests::Intersects(
		XMVectorSet(rayDesc->mRayPos.x, rayDesc->mRayPos.y, rayDesc->mRayPos.z, 1.f),
		XMVectorSet(rayDesc->mRayDir.x, rayDesc->mRayDir.y, rayDesc->mRayDir.z, 0.f),
		v1, v2, v3, rayDesc->mHitDistance))
	{
		//Hit
		if (rayDesc->mRayDistance > rayDesc->mHitDistance)
		{
			XMStoreFloat3(&rayDesc->mRayPos, XMVectorSet(rayDesc->mRayDir.x, rayDesc->mRayDir.y, rayDesc->mRayDir.z, 0.f) * rayDesc->mHitDistance);
			return true;
		}
	}

	return false;
}

_bool CPipeLine::Raycast(_vector v1, _vector v2, _vector v3, RAY_DESC* rayDesc)
{
	return _bool();
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