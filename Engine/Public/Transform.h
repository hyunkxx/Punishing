#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTransform final : public CComponent
{
public:
	enum STATE { STATE_RIGHT, STATE_UP, STATE_LOOK, STATE_POSITION, STATE_END };

public:
	//초당 이동, 회전속도
	typedef struct tagTransformDesc
	{
		_float fMoveSpeed;
		_float fRotationSpeed;
	}TRANSFORM_DESC;

private:
	CTransform(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTransform(const CTransform& rhs);
	~CTransform() = default;
	
public:
	_vector Get_State(STATE eState) { return XMLoadFloat4x4(&m_WorldMatrix).r[eState]; }
	_matrix Get_WorldMatrixInverse() { return XMMatrixInverse(nullptr, XMLoadFloat4x4(&m_WorldMatrix)); }
	_float4x4 Get_WorldMatrix() { return m_WorldMatrix; }
	void Set_WorldMatrix(_float4x4 pWorldMatrix) { m_WorldMatrix = pWorldMatrix; }
	_float3 Get_Scale();
	_float3 Get_Angle() const { return m_fAngle; };
	void Set_Scale(_float3 fScale);
	void Set_State(STATE eState, _fvector vState);
	void Set_TransformDesc(const TRANSFORM_DESC& TransformDesc) { m_TransformDesc = TransformDesc; }

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;

public:
	void MoveForward(_double TimeDelta);
	void MoveBackward(_double TimeDelta);
	void MoveRight(_double TimeDelta);
	void MoveLeft(_double TimeDelta);

	void SetRotationXYZ(_float3 fRadian); // 모든각도 회전
	void SetRotation(_fvector vAxis, _float fRadian); //특정 각도로 회전
	void Rotate(_fvector vAxis, _double TimeDelta);  //현재 각도에서 추가 회전

	void LookAt(_fvector vTargetPos);
	_float Compute_Distance(_fvector vTargetPos);
	void Chase(_fvector vTargetPos, _double TimeDelta, _float fLimitDistance = 0.1f);

public:
	HRESULT Setup_ShaderResource(class CShader* pShaderCom, const char* pConstantName);

public:
	static CTransform* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float3			m_fAngle = { 0.f, 0.f, 0.f };
	_float4x4		m_WorldMatrix;
	TRANSFORM_DESC	m_TransformDesc;

};

END