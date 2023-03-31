#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
class CModel;
END

BEGIN(Client)

// Weapon
class CSwordTrail final : public CGameObject
{
private:
	CSwordTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CSwordTrail() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public:
	void Reset() { m_fTimeAcc = 0.f; }
	void SetOwner(CGameObject* pOwner) { m_pPlayer = (CCharacter*)pOwner; }
	void EffectStart(_float3 vOffsetPos, _float3 fDegreeAngle);

	_bool IsUsed() const { return m_bUse; }

public:
	virtual _float GetLengthFromCamera() override;

public:
	static CSwordTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CModel* m_pModel = nullptr;
	CShader* m_pShader = nullptr;

private:
	CTexture* m_pDiffuse = nullptr;
	CTexture* m_pMask	 = nullptr;

private:
	class CCharacter* m_pPlayer = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

private:
	_bool m_bUse = false;
	const CBone* m_pPivotBone = nullptr;

	_float4x4 EffectWorldMatrix;

	_float m_fTimeAcc = 0.f;
	const _float m_fTimeAccLimit = 1.f;
};

END