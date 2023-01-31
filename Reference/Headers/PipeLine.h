#pragma once

#include "Base.h"

// ��, ���� ����� �����ϴ� ��ü
// �� �����Ӹ��� �� ��ü���� ����� �� �ִ� ��, ��������� ������� ���Ѵ�.
// �� ����� ����� = ī�޶���, ������� ��, ī�޶��� ������ġ�� ���ϰ� �����Ѵ�.

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