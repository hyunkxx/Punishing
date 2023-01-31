#pragma once

#include "Base.h"

// 뷰, 투영 행렬을 보관하는 객체
// 매 프레임마다 각 객체들이 사용할 수 있는 뷰, 투영행렬의 역행렬을 구한다.
// 뷰 행렬의 역행렬 = 카메라의, 월드행렬 즉, 카메라의 월드위치를 구하고 저장한다.

BEGIN(Engine)

class ENGINE_DLL CPipeLine : public CBase
{
	DECLARE_SINGLETON(CPipeLine)

public:
	enum TRANSFORM_STATE { TS_VIEW, TS_PROJ, TS_END };

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