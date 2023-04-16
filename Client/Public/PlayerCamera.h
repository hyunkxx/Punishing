#pragma once
#include "Camera.h"

BEGIN(Engine)
class CBone;
class CCollider;
class CRenderer;
class CTexture;
class CVIBuffer_Rect;
class CShader;
class CLayer;
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
	_bool IsEnding() { return m_bWinAction; }
	_bool IsStarting() { return m_bStartAction; }

	void AttackShake();
	void ThornShake();
	CCollider* GetCollider() { return m_pCollider; }
	void LevelEnd() { m_bFadeIn = true; m_bFadeInStart = true; }

	void EvolutionStart() { m_bEvolution = true; };
	_bool IsEvolition() const { return m_bEvolution; }
	
public:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

private:
	void DefaultCameraMovement(_double TimeDelta);
	void WinActionMovement(_double TimeDelta);
	void StartMovement(_double TimeDelta);
	void EvolutionMovement(_double TimeDelta);

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

	//카메라 움직임 제어
	_bool m_bWinAction = false;
	_float m_fWinActionAcc = 0.f;

	_bool m_bStartAction = true;

	_bool m_isGoal = false;
	_float m_vGoalPosY = 1.5f;
	_float3 m_vCurPos = { 0.f, 0.f, 0.f };

	const _float m_fWinActionTime = 0.5f;

	_bool m_bFinish = false;
	_bool m_bFadeIn = false;
	_bool m_bFadeOut = false;

	_float m_fAlpha = 0.f;

	_bool m_bFadeInStart = false;
	_bool m_bFadeOutStart = false;
	_float m_fFadeInWaitAcc = 0.f;
	_float m_fFadeOutWaitAcc = 0.f;

	_float m_fFadeInAcc = 0.f;
	_float m_fLookAcc = 0.f;

	//변신 무브
	_bool m_bEvolution = false;
	_float m_fEvolutionAcc = 0.f;
	const _float m_fEvolutionLimit = 2.5f;
	_float3 m_vCamStartPos = { 0.f, 0.f, 0.f };
	_float3 m_vCurrentCamPos = { 0.f, 0.f, 0.f };
	_bool m_bCamMoveLock = false;

	_float m_fStartActionAcc = -8.f;
	_float m_fStartDarkAcc = 0.f;
private:
	//Fade in&out
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;

private:
	//Fade in&out
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	CTexture*		m_pTexture = { nullptr };
	
private:
	CLayer* pEnemyLayer = nullptr;
	_bool m_bEvolitionEffectStart = false;
	_bool m_bEnemyLayerSetup = false;
	_bool m_bEvolitionEnemeyAlpha = false;

	_bool m_bFinished = false;
	_float m_fBossBgmStart = 0.15f;


	CShader* m_pTexShader = nullptr;
	_float		m_fLevelX, m_fLevelY, m_fLevelWidth, m_fLevelHeight;
	_float4x4	m_LevelWorldMatrix;
	CTexture* m_pLevel1 = nullptr;
	CTexture* m_pLevel2 = nullptr;

};

END