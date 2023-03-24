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

class CEnemy : 
	public CGameObject, 
	public IOnCollisionEnter,
	public IOnCollisionStay,
	public IOnCollisionExit
{
public:
	enum class TYPE { HUMANOID, ANIMAL };

	typedef struct tagEnemyState
	{
		_float fCurHp;
		_float fMaxHp;
	}ENEMY_STATE;

public:
	enum class CLIP { 
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		ATTACK5,
		ATTACK6,
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
		STUN,
		UISTAND1,
		WALK,
		WALKB,
		WALKL,
		WALKR
	};
	enum class CLIP_TWO
	{
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		BEHITFLY,
		DEATH,
		FALLDOWN,
		HIT1,
		HIT2,
		HIT3,
		HIT4,
		HITDOWN,
		HOVEHIT,
		LEIONFLOOR,
		RUN,
		STAND2,
		STANDUP,
		STUN,
		UISTAND1,
		WALK,
		WALKB,
		WALKL,
		WALKR
	};
public:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Initialize_Prototype(TYPE eType);
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

	virtual _float4 GetPosition();
public:
	_fmatrix GetWorldMatrix();

	void LookPlayer();
	void SetupCamera(class CPlayerCamera* pCamera) { m_pCamera = pCamera; };

	_float2 Reset(_float3 vPos, _float fRadius);
	_bool IsOverlap() const { return m_bOverlapped; }
	void SetOverlap(_bool value, _float3 vNagative) { m_bOverlapped = value; m_vNagative = vNagative; }
	void SetPosition(_float3 vPosition);
	_vector GetRootBonePosition();
	virtual class CCollider* GetBodyCollider() { return collider; }
	virtual class CCollider* GetWeaponCollider() { return m_pWeaponCollider; }
	virtual class CCollider* GetOverlapCollider() { return m_pOverlapCollider; }
	void SetNuckback(_float fPower);
	void SetHold(_bool value) { m_bHolding = value; }
	void SetAirborne(_float fDamage);
	ENEMY_STATE GetHpState() const { return m_State; }

	void RecvDamage(_float fDamage);

	//죽는중
	_bool IsDeadWait() const { return m_bDeadWait; }
protected:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

protected:
	void OverlapProcess(_double TimeDelta);
	void LookPlayer(_double TimeDelta);
	void Trace(_double TimeDelta);
	void Attack(_double TimeDelta);
	void DisableAttackCollision(_double TimeDelta);
	void Idle(_double TimeDelta);
	_bool Hit(_double TimeDelta);

	void NuckBack(_double TimeDelta);
	void Airborne(_double TimeDelta);
	_bool IsAirbone() { return m_bAir; };
	void AirboneReset();

	void Holding(_double TimeDleta);

	_bool DieCheck();
	void Die(_double TimeDelta);

	_double Freeze(_double TimeDelta);
private:
	void AnimationState(_double TimeDelta);

public:
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest);
	virtual void OnCollisionStay(CCollider * src, CCollider * dest);
	virtual void OnCollisionExit(CCollider * src, CCollider * dest);

	virtual _float GetLengthFromCamera() override;

public:
	static CEnemy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

protected:
	CRenderer* renderer = nullptr;
	CTransform* transform = nullptr;
	CModel* model = nullptr;
	CShader* shader = nullptr;
	CCollider* collider = nullptr;

	//몬스터 끼리 밀어내기용
	CCollider* m_pOverlapCollider = nullptr;

	CCollider* m_pWeaponCollider = nullptr;
	CBone* m_pWeaponBone = nullptr;

	class CApplicationManager* m_pAppManager = nullptr;
protected:
	static _uint s_iCount;

protected:
	TYPE m_eType = TYPE::HUMANOID;

	_float4x4 m_RootBoneMatrix;
	class CBone* bone = nullptr;
	_uint animation = 0;

	ENEMY_STATE m_State;

	CCharacter* m_pPlayer = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

	_bool m_bTraceFinish = false;
	_bool m_bRotationFinish = false;

	const _float m_fAttackRange = 3.5f;

	//Attack
	_bool m_bAttack = false;
	_float m_fAttackCollisionLocal = 0.0f;
	const _float m_fAttackCollisionTimeOut = 0.1f;

	//Overlap
	_float3 m_vNagative = { 0.f, 0.f, 0.f };
	_bool m_bOverlapped = false;
	_double m_OverlapAcc = 0.0;
	const _double m_OverlappedWait = 1.f;

	_bool m_bMovable = false;

	//Die
	_bool m_bDead = false;
	_bool m_bDeadWait = false;
	_float m_fDeadWaitTimer = 0.0f;
	const _float m_fDeadWaitTimeOut = 2.f;

	//Hit
	_bool m_bAttackOneCall = false;
	_bool m_bAttackCollision = false;
	_bool m_bHitStart = false;
	_bool m_bHit = false; //플레이어한테 맞았을때 true

	_bool m_bNuckback = false;
	_bool m_bNuckBackFinish = false;
	_float m_fNuckbackPower = 8.0f;
	_float m_fNuckBackTimer = 0.0f;
	const _float m_fNuckBackTimeOut = 0.15f;

	_float m_fCurTimeScale = 1.0;

	_float m_fAttackCoolTimer = 4.0f;
	const _float m_fAttackCoolTimeOut = 4.0f;

	_float m_fTraceLocal = 0.0f;
	const _float m_fTraceTimeOut = 0.5f;


	// 에어본
	_bool m_bAirHit = false;
	_bool m_bAir = false;

	_bool m_bStandupStart = false;
	_float m_fStandupTimer = 0.0f;
	const _float m_fStandupTimeOut = 2.0f;

	//플레이어한테 잡힘
	_bool m_bHolding = false;

	_int m_iRandomHitAnim = 0;

	_bool m_bAlpha = false;
	class CPlayerCamera* m_pCamera;

	_float m_fAirboneAcc = 0.f;
	const _float m_fAirboneTimeOut = 2.f;

	_bool m_bSpawnWait = true;
	_float m_fSpawnWaitAcc = 0.f;
	_float m_fSpawnWaitTimeOut = 2.f;
};

END