#pragma once

#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CSphereCollider;
class IOnCollisionEnter;
class IOnCollisionStay;
class IOnCollisionExit;
class CTransform;
class CVIBuffer_Rect;
class CRenderer;
class CShader;
class CTexture;
END
	
BEGIN(Client)

class CEnemySpawner final : 
	public CGameObject,
	public IOnCollisionEnter
{
	enum { FIRST_SPAWNER, SECOND_SPAWNER };
protected:
	explicit CEnemySpawner(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemySpawner(const CGameObject& rhs);
	virtual ~CEnemySpawner() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

public:
	//스폰 위치 및 반경
	void SetupCamera(class CPlayerCamera* pCam) { m_pPlayerCamera = pCam; }
	void SetupSpawner(_float3 vPos, _float fRadius);

private:
	HRESULT AddComponents();

private:
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;

public:
	static CEnemySpawner* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CTransform* m_pTransform = nullptr;
	CSphereCollider* m_pCollider = nullptr;

	_float m_fRadius = 8.f;

	_bool m_isEnter = false;

	_int m_iCurSpawnerIndex = 0;
	_int m_iRespawnCount = 0;

	class CPlayerCamera* m_pPlayerCamera = nullptr;

	_float m_fDisAcc = 0.f;
	CTransform* m_pWarpTransform = nullptr;
	CVIBuffer_Rect* m_pWarpBuffer = nullptr;
	CRenderer* m_pRenderer = nullptr;
	CTexture* m_pWarpNoise = nullptr;
	CShader* m_pShader = nullptr;

	_float m_fBGMVolum = 0.1f;
};

END