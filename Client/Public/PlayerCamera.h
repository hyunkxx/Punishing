#pragma once
#include "Camera.h"

BEGIN(Engine)
class CBone;
class CCollider;
END

BEGIN(Client)

class CPlayerCamera final : public CCamera
{
private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CPlayerCamera& rhs);
	virtual ~CPlayerCamera() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void AttackShake();
	CCollider* GetCollider() { return m_pCollider; }
public:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CGameObject* m_pTarget = nullptr;
	CTransform* m_pTargetTransform = nullptr;
	CTransform* m_pSocketTransform = nullptr;
	CCollider* m_pCollider = nullptr;

	_bool m_bMouseLock = true;

	CBone* m_pBone = nullptr;
	_float4x4 m_cameraReference;
	_float4 vLookTarget;
};

END