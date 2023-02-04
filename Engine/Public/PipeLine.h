#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CPipeLine : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORM_STATE { TS_VIEW, TS_PROJ, TS_END };

	struct RAY_DESC
	{
		_float3 mRayPos;
		_float3 mRayDir;
		_float3 mHitPos;
		_float mRayDistance = 1000.f;
		_float mHitDistance;
	};

	struct CLIENT_DESC
	{
		HWND hWnd;
		POINT mViewportSize;
	};

private:
	CPipeLine() = default;
	virtual ~CPipeLine() = default;

public:
	_float4x4 Get_Transform_float4x4(TRANSFORM_STATE eState);
	_float4x4 Get_Transform_float4x4_Inverse(TRANSFORM_STATE eState);
	_matrix Get_Transform_Matrix(TRANSFORM_STATE eState);
	_matrix Get_Transform_Matrix_Inverse(TRANSFORM_STATE eState);
	_float4 Get_CamPosition();

public:
	void Set_Transform(TRANSFORM_STATE eState, _fmatrix TransformMatrix);
	_bool RaycastFromCusor(_vector v1, _vector v2, _vector v3, RAY_DESC* rayDesc, const CLIENT_DESC& ViewportSize);
	_bool Raycast(_vector v1, _vector v2, _vector v3, RAY_DESC* rayDesc);

public:
	HRESULT Initialize();
	void Tick();

public:
	virtual void Free() override;

private:
	_float4x4 m_TransformMatrix[TS_END];
	_float4x4 m_TransformMatrixInverse[TS_END];
	_float4 m_vCamPosition;

};

END