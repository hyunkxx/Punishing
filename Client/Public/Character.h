#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Collider.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
class CTimer;
class CCollider;

class IOnCollisionStay;
END

BEGIN(Client)
class CEnemy;

class CCharacter final : 
	public CGameObject, 
	public IOnCollisionEnter,
	public IOnCollisionStay,
	public IOnCollisionExit
{
public:
	/*enum CLIP
	{
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		ATTACK11,
		ATTACK21,
		ATTACK31,
		ATTACK51,
		ATTACK_QTE,
		HIT_FLY,
		ACTION0,
		ACTION1,
		ACTION2,
		BORN,
		DEATH,
		FALLDOWN,
		HIT1,
		HIT2,
		HIT3,
		HIT4,
		HITDOWN,
		HOVEHIT,
		LIEONFLOOR,
		MOVE1,
		MOVE2,
		RUN,
		RUN_LEFT,
		RUN_RIGHT,
		RUN_START,
		RUN_START_END,
		SAVE,
		STAND1,
		STAND2,
		STAND_ACTION,
		STAND_CHANGE,
		STAND_UP,
		STOP, // Run End 같은데 확인해야함
		STUN,
		UI_STAND,
		CLIP_END

		칼레리나
	};*///칼레니나 클립

	enum CLIP
	{
		ATTACK1,
		ATTACK2,
		ATTACK3,
		ATTACK4,
		ATTACK5,
		ATTACK11,
		ATTACK12,
		ATTACK21,
		ATTACK22,
		ATTACK31,
		ATTACK32,
		ATTACK41,
		ATTACK42,
		ATTACK43,
		ATTACK44,
		ATTACK45,
		ATTACK51,
		ATTACK_QTE,
		HIT_FLY,
		ACTION0,
		ACTION1,
		ACTION2,
		BORN,
		DEATH,
		FALLDOWN,
		HIT1,
		HIT2,
		HIT3,
		HIT4,
		HITDOWN,
		HOVEHIT,
		LIEONFLOOR,
		MOVE1,
		MOVE2,
		RUN,
		RUN_LEFT,
		RUN_RIGHT,
		RUN_START,
		RUN_START_END,
		SAVE,
		STAND1,
		STAND2,
		STAND_ACTION,
		STAND_CHANGE,
		STAND_UP,
		STOP, // Run End 같은데 확인해야함
		STUN,
		UI_STAND,
		CLIP_END


	};

	typedef struct tagOverlapInfo
	{
		_float3 vDir;
		_float fDepth;
	}OVERLAP_INFO;

private:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	const CBone* GetBone(const char* szBoneName) const;

private:
	HRESULT AddWeapon();
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

	_bool AnimationCompare(CLIP eClip);
	_bool FinishCheckPlay(CLIP eClip, CAnimation::TYPE eAnimationType);
	void SetAnimation(CLIP eClip, CAnimation::TYPE eAnimationType);
	void AnimationControl(_double TimeDelta);

private://Camera

	void CameraSocketUpdate();
	void ForwardRotaion(_double TimeDelta);
	void BackwardRotaion(_double TimeDelta);
	void RightRotation(_double TimeDelta);
	void LeftRotation(_double TimeDelta);
	void Movement(_double TimeDelta);

private:
	void KeyInput(_double TimeDelta);
	void Dash(_double TimeDelta);

	void Idle();
	void InputMove(_double TimeDelta);
	void MoveStop(_double TimeDelta);
	void Attack(_double TimeDelta);
	void PositionHold(_double TimeDelta);

public: // Enemy 관련 코드
	CGameObject* GetLockOnTarget() { return m_pNearEnemy != nullptr ? (CGameObject*)m_pNearEnemy : nullptr; }
	_bool IsCameraLockOn() { return m_pNearEnemy != nullptr; };
	void TargetListDeastroyCehck();
	_bool FindTargetFromList(CGameObject* pObject);
	void DeleteTargetFromList(CGameObject* pObject);
	void FindNearTarget();
	void NearTargetChange();

	_float3 LockOnCameraPosition();

public: //충돌관련
	CCollider* GetWeaponCollider() const { return mWeaponCollider; }

public:
	static CCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

public:
	virtual void OnCollisionEnter(CCollider* src, CCollider* dest);
	virtual void OnCollisionStay(CCollider* src, CCollider* dest);
	virtual void OnCollisionExit(CCollider* src, CCollider* dest);

private:
	CRenderer* mRenderer = nullptr;
	CTransform* mTransform = nullptr;
	CModel* mModel = nullptr;
	CShader* mShader = nullptr;
	CCollider* mCollider = nullptr;

	CCollider* mEnemyCheckCollider = nullptr;
	CCollider* mWeaponCollider = nullptr;

private: //레이어 삭제시 삭제됨
	class CWeapon* m_pWeapon;
	class CBone* bone = nullptr;
	class CBone* m_pWeaponBone = nullptr;

	CAnimation::ANIMATION_DESC ANIM_DESC;

private: // camera
	CTransform* mCameraSocketTransform = nullptr;
	_bool m_bCameraBack = true;

private: // Command
	_bool m_bCombatMode = false;

	const _uint m_iAttackCount = 5;
	_uint m_iCurAttackCount = 0;
	_bool m_bAttacking = false;
	_bool m_bAttackable = true;

	_bool m_bMoveable = true;

	_int m_iWASDCount = 0;
	_bool m_bMove = false;
	_bool m_bRun = false;
	_bool m_bOnTerrain = true;

	_float m_fMoveSpeed = 5.f;
	_float m_fRotationSpeed = 360.f;
	const _double m_fStopTimeOut = 0.15f;
	_double m_fStopTimer = 0.0;

	_bool m_bDashable = true;
	_bool m_bFrontDashReady = false;
	_bool m_bLeftDashReady = false;
	_bool m_bRightDashReady = false;
	const _double m_fDashTimeOut = 0.1;
	_double m_fDashFrontTimer = 0.0;
	_double m_fDashLeftTimer = 0.0;
	_double m_fDashRightTimer = 0.0;

	// 몬스터
	_uint m_iEnemyIndex = 0;
	list<CEnemy*> m_Enemys;
	CEnemy* m_pNearEnemy = nullptr;

	// 무기 충돌처리
	_int m_iSecoundAttackCount = 0;
	_bool m_bWeaponTimerOn = false;
	_float m_fWeaponCollisionAcc = 0.0f; 
	const _float m_fWeaponCollDelay = 0.5f;

	// 충돌체 충돌시 제자리 홀드
	_bool m_bHolding = false;
	vector<OVERLAP_INFO> m_OverlappedInfo;

};

END

//XMQuaternionSlerp