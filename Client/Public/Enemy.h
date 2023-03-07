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

public:
	_bool IsOverlap() const { return m_bOverlapped; }
	void SetOverlap(_bool value, _float3 vNagative) { m_bOverlapped = value; m_vNagative = vNagative; }
	_float4 GetPosition();
	void SetPosition(_float3 vPosition);
	_vector GetRootBonePosition();

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

private:
	void OverlapProcess(_double TimeDelta);
	void LookPlayer(_double TimeDelta);
	void Trace(_double TimeDelta);
	void Attack(_double TimeDelta);
	void Idle(_double TimeDelta);
	_bool Hit(_double TimeDelta);
	void NuckBack(_double TimeDelta);

private:
	void AnimationState(_double TimeDelta);

public:
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest);
	virtual void OnCollisionStay(CCollider * src, CCollider * dest);
	virtual void OnCollisionExit(CCollider * src, CCollider * dest);

public:
	static CEnemy* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, TYPE eType);
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
	TYPE m_eType = TYPE::HUMANOID;

	_float4x4 m_RootBoneMatrix;
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

	//Hit
	_bool m_bHit = false; //플레이어한테 맞았을때 true
	_bool m_bNuckBackFinish = false;
	_float m_fNuckBackTimer = 0.0f;
	const _float m_fNuckBackTimeOut = 0.2f;
};

END