#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Collider.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CBone;
class CShader;
class CTimer;
class CCollider;

END

BEGIN(Client)

class CCharacter;

class CEnemy final : 
	public CGameObject, 
	public IOnCollisionEnter,
	public IOnCollisionStay,
	public IOnCollisionExit
{
public:
	typedef struct tagEnemyState
	{
		_float fCurHp;
		_float fMaxHp;
	}ENEMY_STATE;

public:
	enum CLIP { 
		ATTACK1,
		ATTACK2,
		ATTACK3,
		BEHITFLY,
		DEATH,
		FALLDOWN,
		HIT1,
		HIT2,
		HIT3,
		HIT4,
		HITDOWN,
		HOVERHIT,
		LIEONFLOOR,
		RUN,
		RUNB,
		RUNL,
		RUNR,
		STAND,
		STANDUP,
	};

public:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	_bool IsOverlap() const { return m_bOverlapped; }
	void SetOverlap(_bool value, _float3 vNagative) { m_bOverlapped = value; m_vNagative = vNagative; }
	_float4 GetPosition();
	void SetPosition(_float3 vPosition);

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

private:
	void OverlapProcess(_double TimeDelta);
	void LookPlayer(_double TimeDelta);
	void Trace(_double TimeDelta);
	void Attack(_double TimeDelta);
	void Idle(_double TimeDelta);

private:
	void AnimationState(_double TimeDelta);

public:
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest);
	virtual void OnCollisionStay(CCollider * src, CCollider * dest);
	virtual void OnCollisionExit(CCollider * src, CCollider * dest);

public:
	static CEnemy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* renderer = nullptr;
	CTransform* transform = nullptr;
	CModel* model = nullptr;
	CShader* shader = nullptr;
	CCollider* collider = nullptr;

private:
	static _uint s_iCount;

private:
	class CBone* bone = nullptr;
	_uint animation = 0;

	ENEMY_STATE m_State;

	CCharacter* m_pPlayer = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

	_bool m_bRotationFinish = false;

	const _float m_fAttackRange = 3.0f;
	//Attack
	_bool m_bAttack = false;

	//Overlap
	_float3 m_vNagative = { 0.f, 0.f, 0.f };
	_bool m_bOverlapped = false;
	_double m_OverlapAcc = 0.0;
	const _double m_OverlappedWait = 1.f;

	_bool m_bMovable = false;
};

END