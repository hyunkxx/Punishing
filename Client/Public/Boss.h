#pragma once

#include "GameObject.h"
#include "Enemy.h"
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
class CSphereCollider;

class IOnCollisionEnter;
class IOnCollisionStay;
class IOnCollisionExit;
END

BEGIN(Client)
class CCharacter;

class CBoss final 
	: public CEnemy
{
public:
	enum BOSS_CLIP
	{
		ATK_FOWARD,
		ATK_BACK,
		ATK3,
		ATK4,
		ATK5,//변신
		ATK11,
		ATK12,//ㅌ
		ATK13,
		UNKNOWN1,
		UNKNOWN2,
		UNKNOWN3,
		UNKNOWN4,
		UNKNOWN5,
		UNKNOWN6,
		BORN,
		DEATH,
		STANDEX,
		STAND1,
		STAND2,
		UISTAND,
		MOTION_END
	};

	typedef struct tagAnimState
	{
		BOSS_CLIP eCurAnimationClip;
		CAnimation::TYPE eAnimType;
		_bool bLerp;
	}ANIM_STATE;

public:
	explicit CBoss(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CBoss(const CBoss& rhs);
	virtual ~CBoss() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public://엑세스
	_bool IsBurrow() const { return m_bBrrow; }
	_bool IsSpawned() const { return m_bSpawn; }

public://스킬
	void Spawn();
	_bool IsDie() { return m_bDie; };
	void MoveForward(_double TimeDelta);
	void MoveBackword(_double TimeDelta);
	void LineSkill(_double TimeDelta, _int iIndex);
	void Missile1(_double TimeDelta);
	void CloseAttack(_double TimeDelta);
	void Burrow(_double TimeDelta);
	void Evolution(_double TimeDelta);

	//2페이즈 기본 근접공격
	void ColseAttack2(_double TimeDelta);
	void LineSkill2(_double TimeDelta);
	void LastAttack(_double TimeDelta);

public: //플레이어 관련
	_float GetLengthFromPlayer() const;
	_bool CloseToPlayer() { return GetLengthFromPlayer() <= m_fNearCheckRange; };
	_bool LookTarget(_double TimeDelta, _float fRotationSpeed);

private://애니메이션 관련
	void SetupState(BOSS_CLIP eClip, CAnimation::TYPE eAnimType, _bool bLerp) 
	{ 
		m_eAnimState.eCurAnimationClip = eClip;
		m_eAnimState.eAnimType = eAnimType;
		m_eAnimState.bLerp = bLerp;
	}

	void DefaultAnimation(_double TimeDelta);
	void EvolutionAnimation(_double TimeDelta);
	void AnimationController(_double TimeDelta);

private://콜라이더 관련
	void SetupColliders();

public:
	virtual _float4 GetPosition() override;
	virtual _float GetLengthFromCamera() override;

	//Collider
	virtual class CCollider* GetBodyCollider() override { return collider; }
	virtual class CCollider* GetWeaponCollider() override  { return m_pWeaponCollider; }
	virtual class CCollider* GetOverlapCollider() override  { return m_pOverlapCollider; }

	//virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	//virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	//virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:


	_int iAnim = 0;
	ANIM_STATE m_eAnimState;
	
	_bool m_bRotationFinish = false;
	_bool m_bAttackable = false;
	_int m_iAttackCount = 0;

	_bool m_bSpawn = false;
	const _float m_fNearCheckRange = 18.f;	 // 플레이어와 가까운지 체크 10

	_bool m_bBrrow = false;

	//라인 3타 공격
	_bool m_bLineAttack = false;

	class CThorn* m_pThorn1[25][6];
	class CThorn* m_pThorn2[25][6];

	_float3 vThornSpawnPos = { 0.f, 0.f, 0.f };

	_int m_iCurrentOneIndex[3] = {0, 0, 0};
	_int m_iCurrentTwoIndex[3] = {0, 0, 0};

	_int m_iEraseIndex[3] = { 0, 0, 0 };

	_bool m_bUseLineSkill = false;
	_bool m_bLineSkillStart[6] = { false, false, false };
	_bool m_bLineSkillErase[6] = { false, false, false };
	
	float m_fEraseAcc[3] = { 0.f, 0.f, 0.f };
	const float m_fEraseTime = 0.05f;

	_bool m_bCheckDir[3] = { false, false, false };
	_float3 m_vLineOneDir[3];

	_bool m_bEvolution = false;

	//라인 3타 공격 - 총 3회중 몇번쨰 라인인지
	_int m_iCurrentLine = 0;
	_float m_fLineUpAcc = 0.f;
	const _float m_fLineUpTime = 0.5f;
	
	//보스 몸체 위로 올리기위한 트렌스폼
	CTransform* m_pBodyTransform = nullptr;

	_bool m_bMoveForward= false;
	_bool m_bMoveBackward= false;

	//뒤로 이동 ATK_BACK
	_bool m_bBack = false;
	_float3 m_vPrevPos = { 0.f,0.f,0.f };
	_float3 m_vPrevLook = { 0.f, 0.f, 0.f };

	_float m_fAttackAcc = 2.f;
	const _float m_fAttackAccTime = 2.f;

	//미사일 9개 \ | / 방향으로 날리기
	class CThorn* m_pThornMissileLeft[3];
	class CThorn* m_pThornMissileMiddle[3];
	class CThorn* m_pThornMissileRight[3];

	_bool m_bUseMissile1 = false;
	_bool m_bMissileStart = false;

	//플레이어와 멀어졌을때
	_bool m_bTooFar = false;
	_float m_fFarAcc = 0.f;
	const _float m_fFarTime = 2.f;

	//버로우 기능 근접, 디폴트포스
	_bool m_bBurrowable = true; //다른 공격중 버로우 불가
	_bool m_bBurrowStart = false;
	_float m_fBurrowAcc = 0.f;

	//근접공격
	_bool m_bCloseAttackStart = false;
	_bool m_bCloseAttack = false;
	_float m_fCloseAttackActiveAcc = 0.f;
	const _float m_fCloseAttackActiveTime = 3.f;
	class CThorn* m_pThornClose[9];

	_bool m_bEvolutionFinish = false;
	_bool m_bEvolutionStart = false;

	//2페이즈 근접공격
	class CThorn* m_pThornCloseRightFront[9];
	class CThorn* m_pThornCloseLeftFront[9];
	class CThorn* m_pThornCloseFront[9];
	class CThorn* m_pThornCloseRight[9];
	class CThorn* m_pThornCloseLeft[9];
	
	//프론트, 전방양옆, 양옆
	_bool m_bCloseLockTarget = false;
	_bool m_bCloseAttackExBegin = false;
	_bool m_bCloseAttackExStart = false;

	_int m_iColseAttackIndex = 0;
	_bool m_bColseAttackExStart[2] = { false, false };
	_bool m_bColseAttackEx[2] = { false, false };

	_float3 vPrevPlayerPos = { 0.f ,0.f, 0.f };

	_float m_fNextIndexAcc = 0.f;
	const _float m_fNextIndexTime = 0.5f;

	//라인공격 강화 제거
	_bool m_bLineAttackEraseEx = false;
	_float m_fEraseExAcc = 0.0f;
	const _float m_fEraseExDelay = 0.05f;
	_int m_iEraseIndexEx = 0;

	_float3 m_vFrontDir = { 0.f, 0.f, 0.f };
	_float3 m_vLeftDir = { 0.f, 0.f, 0.f };
	_float3 m_vRightDir = { 0.f, 0.f, 0.f };

	_float3 m_vBackDir = { 0.f, 0.f, 0.f };
	_float3 m_vLeftBackDir = { 0.f, 0.f, 0.f };
	_float3 m_vRightBackDir = { 0.f, 0.f, 0.f };

	_int m_iAttackCountEx = 0;
	_bool m_bLastAttackBegin = false;
	_bool m_bLastAttack = false;
	_float m_fLastAttackEraseAcc = 0.f;
	const _float m_fLastAttackEraseDelay = 0.01f;

	int m_iLastEraseIndex[12] = {0,};
	_bool m_bLineExAnimStart = false;
	_bool m_bLineExAnimSetup = false;
	_bool m_bCloseExAnimStart = false;
	_bool m_bCloseExAnimSetup = false;

	enum { LEFT, RIGHT, FRONT, BACK, BACK_LEFT, BACK_RIGHT, COLLTRANS_END };
	enum { LEFT_COLL, RIGHT_COLL, FRONT_COLL, BACK_COLL, BACK_LEFT_COLL, BACK_RIGHT_COLL, COLL_END };
	enum { ONE, TWO, TRE };
	CTransform* m_pColliderTransform[COLLTRANS_END];
	CTransform* m_pColliderCloseTransform[3];

public:
	CCollider* GetCloseAttackCollider(_int iIndex) { return m_pCloseAttack[iIndex]; };
	CCollider* GetLineAttackCollider(_int iIndex) { return m_pColliderLine[iIndex]; };

private:
	CCollider* m_pColliderLine[6];
	CCollider* m_pCloseAttack[3];
	_bool m_bLineAttackCollActive = false;
	_bool m_bLineAttackExCollActive = false;
	_bool m_bCloseAttackCollActive = false;
	const _float m_fLineCollSpeed = 10.f;
	//_bool m_bLineAttackCollStart[3] = { false, false, false };

	_bool m_bDie = false;
	_bool m_bDead = false;
	_bool m_bRender = true;
	_float m_fDieWaitAcc = 0.f;
	const _float m_fDieWaitTime = 4.f;

	//쉐이크
	_bool m_bMissileShake = false;
};

END
