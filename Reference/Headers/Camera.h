#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc
	{
		CTransform::TRANSFORM_DESC TransformDesc;

		_float3 vEye;
		_float3 vAt;
		_float3 vAxisY;

		_float fFovy;
		_float fAspect;
		_float fNear;
		_float fFar;

	}CAMERA_DESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual	HRESULT Initialize_Prototype();
	virtual	HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void StartShake(_float Time, _float fPower, _float fPowValue = 0.2f);
	void ShakeReset() { m_fShakeTimer = 0.0f; }
	void Shake(_double TimeDelta);

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;

protected:
	class CTransform*	m_pTransform = { nullptr };
	class CPipeLine*	m_pPipeLine = { nullptr };
	CAMERA_DESC			m_CameraDesc;

	//카메라 쉐이킹
	_bool m_bShake = false;
	_float m_fPower = 3.f;
	_float m_fPowValue = 0.2f;
	_float m_fShakeTimer = 0.f;
	_float m_fShakeTimeOut = 1.f;
	_float4x4 m_PrevCamPos;
	_float m_fPrevFOV;
	_float m_fLocalTimeAcc = 0.f;

};

END