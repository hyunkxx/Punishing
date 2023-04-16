#pragma once

#include "GameObject.h"
#include "Animation.h"
#include "Collider.h"
#include "SkillBase.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
class CTimer;
class CCollider;
class CTexture;
class CVIBuffer_Rect;

class IOnCollisionStay;
END

BEGIN(Client)
class CEnemy;
class CWall;

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
		STOP, // Run End ������ Ȯ���ؾ���
		STUN,
		UI_STAND,
		CLIP_END

		Į������
	};*///Į���ϳ� Ŭ��

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
		STOP, // Run End ������ Ȯ���ؾ���
		STUN,
		UI_STAND,
		WIN,
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

	virtual _float GetLengthFromCamera() override;
public:
	CTransform* GetTransform() { return mTransform; }
	void SetPosition(_float3 vPos);
	_fmatrix GetTargetMatrix();
	_float2 GetTargetWindowPos();
	void SetPlayerCamera(class CPlayerCamera* pCamera) { m_pCamera = pCamera; };
	const CBone* GetBone(const char* szBoneName) const;
	void SetHealthUI(class CEnemyHealthBar* value) { m_pEnemyHealthBar = value; }

	void LookPos(_fvector vLookPos);

	_int GetComboCount() { return m_iComboCount; }
	void ResetComboTime() { m_fCurComboTimer = 0.f; }
	void AddCombo() { m_iComboCount++; }

	void AddEvolutionGage(CSkillBase::SKILL_INFO);
	_float GetCurEvolutionTime() const { return m_fCurEvolutionAcc; }
	_float GetEvolutionTime() const { return m_fEvolutionTimeOut; }

	_bool IsCurrentActionEvolution();
	_bool IsCurrentActionEvolutionFinish();

	_bool IsEvolutionReady() { return m_bEvolutionReady; }
	_bool IsEvolution() { return m_bEvolution; }
	_bool IsAttackalbe() { return m_bAttackable; }
	_bool IsDashable() { return m_bDashable; }
	_bool IsDashGageFull() { return m_fCurDash >= 20.f; }

	void StartEvolitionEffect();
	void UseFootSmoke();
	void SetWinMotion(_bool value) { m_bWin = value; };

	//�ܻ�
	void CurrentMotionCapture();
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
	void AttackFootSmoke(_double TimeDelta);
	void PositionHold(_double TimeDelta);

	void SkillA(_double TimeDelta);
	void SkillB(_double TimeDelta);
	void SkillC(_double TimeDelta);
	void SkillColliderControl(_double TimeDelta);


public: // Enemy ���� �ڵ�
	void ClearEnemyCheckCollider() { mEnemyCheckCollider->HitColliderReset(); };
	CGameObject* GetLockOnTarget() { return m_pNearEnemy != nullptr ? (CGameObject*)m_pNearEnemy : nullptr; }
	_bool IsCameraLockOn();
	void TargetListDeastroyCehck();
	_bool FindTargetFromList(CGameObject* pObject);
	void DeleteTargetFromList(CGameObject* pObject);
	void FindNearTarget();
	void NearTargetChange();

	_float3 LockOnCameraPosition();
	void HoldEnemy();

	void Hit();
	void Airbone();
	void AirboneProcess(_double TimeDelta);
	void RecvDamage(_float fDamage);
	_double Freeze(_double TimeDelta);

	void RenderEnemyHealth(_double TimeDelta);
	_float GetDamage();

	//�ʻ����
public:
	void SavePrevPos();
	_bool IsBornFinish() { return m_bBornFinish; }
private:
	_bool m_bBornFinish = false;

public: //�浹����
	class CCollider* GetBodyCollider() const { return mCollider; };
	CCollider* GetWeaponCollider() const { return mWeaponCollider; }
	CCollider* GetSkillCollider() const { return mSkillCollider; }
	CCollider* GetEnemyCheckCollider() const { return mEnemyCheckCollider; }

	_double GetTimeDelta() const;

public://����Ʈ ����
	class CSwordTrail* GetNotUsedEffect();
	void UseSwordEffect(_float3 vOffsetPos, _float3 fDegreeAngles);
	void AttackEffectControl(_double TimeDelta);
	_bool IsEvolutionFinish() const { return m_bEvolitionMotionFinish; }
	void CalcFootHeight();

public:
	static CCharacter* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

	_bool IsStartMotion();

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

	CCollider* mDashCheckCollider = nullptr;
	CCollider* mWallCheckCollider = nullptr;
	CCollider* mEnemyCheckCollider = nullptr;
	CCollider* mWeaponCollider = nullptr;

	CCollider* mSkillCollider = nullptr;

private:
	class CApplicationManager* m_pAppManager = nullptr;

private: //���̾� ������ ������
	class CWeapon* m_pWeapon;
	class CBone* bone = nullptr;
	class CBone* m_pWeaponBone = nullptr;

	CAnimation::ANIMATION_DESC ANIM_DESC;

private: // camera
	CTransform* mCameraSocketTransform = nullptr;
	_bool m_bCameraBack = true;

private: // Command
	_bool m_bStart = true;
	_bool m_bStartAction = false;

	_bool m_bCombatMode = false;

	CLIP m_iPrevClip = CLIP::ACTION0;
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
	_float m_fMoveLowSpeed = 1.f;
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

	// ����
	_uint m_iEnemyIndex = 0;
	list<CEnemy*> m_Enemys;
	CEnemy* m_pNearEnemy = nullptr;

	// ���� �浹ó��
	_int m_iSecoundAttackCount = 0;
	_bool m_bHitColliderCheck = false;
	_bool m_bWeaponTimerOn = false;
	_float m_fWeaponCollisionAcc = 0.0f; 
	const _float m_fWeaponCollDelay = 0.5f;

	// �浹ü �浹�� ���ڸ� Ȧ��
	_bool m_bHolding = false;
	vector<OVERLAP_INFO> m_OverlappedInfo;

	//���� �����ӿ��� ��ġ
	CWall* m_pNearWall = nullptr;
	_float m_fNearPlaneLength = 0.f;
	_bool m_WallHit = false;
	_float3 vPrevPosition;

	//��ȭ����
	_bool m_bEvolution = false;
	_bool m_bEvolitionMotionFinish = false;
	_bool m_bEvolutionAttack = false;

	//ȸ�� (�뽬 �ݸ���)
	_bool m_bDashPrevPosSet = false;
	_float4x4 m_matrixPrevPos;
	_float m_fActiveDuration = 0.0f;
	const _float m_fActiveTimeOut = 0.5f;

	//���� ����
	_bool m_bDie = false;
	_bool m_bHit = false;
	_float m_fCurHp = 1000.f;
	_float m_fMaxHp = 1000.f;

	_bool m_bTimeStop = false;
	_double m_fCurTimeScale = 1.;
	_double m_fTimeScale = 1.0;
	_double m_fTimeStopLocal = 0.0;
	const _float m_fTimeStopTimeOut = 12.f;
	
	_bool m_bSkillReady = true;
	_bool m_bUseSkill = false;

	_bool m_bRootMotion = true;
	
	//���� ���
	_bool m_bSkillYellowAttack = false;
	_bool m_bEnemyHolding = false;

	//UI
	class CEnemyHealthBar* m_pEnemyHealthBar = nullptr;
	_bool m_bEnemyHealthDraw = false;
	_float m_fDrawEnemyHealthTimer = 0.f;
	const _float m_fDrawEnemyHealthTimeOut = 8.f;

	CSkillBase::SKILL_INFO m_SkillInfo;

	_float m_fCurDash = 50.f;
	_float m_fMaxDash = 100.f;

	class CPlayerHealthBar* m_pHealthBar = nullptr;
	class CPlayerCamera* m_pCamera = nullptr;

	_bool m_bFreezeReadyWait = false;
	_float m_fFreezeReadyTimer = 0.f;
	const _float m_fFreezeReadyTimeOut = 8.f;

	class CPlayerIcon* m_pPlayerIcon = nullptr;
	_int m_iComboCount = 0;
	_float m_fCurComboTimer = 0.f;
	const _float m_fComboTimeOut = 3.5f;

	//���Ż��� 10�� ����
	_int m_EvolutionCount = 0;
	_bool m_bGageDownStart = false;
	_bool m_bEvolutionReady = false;
	_float m_fCurEvolutionAcc = 0.f;
	const _float m_fEvolutionTimeOut = 25.f;

	//�г� ������ �� �����ϋ� ��ų ��
	_bool m_bRedAddGageReady = false;
	_bool m_bYellowAddGageReady = false;
	_bool m_bBlueAddGageReady = false;

	_bool m_bWin = false;
	_bool m_bOneAction = false;

	//���
	_bool m_bLanding = false;
	_bool m_bAirbone = false;
	_bool m_vDirUp = true;
	_float m_fAirboneAcc = 0.f;
	const _float m_fAirboneTime = 1.f;
	_float3 m_vPrevLook = { 0.f, 0.f, 1.f };
	_bool m_bStandUp = false;

	CEnemy* m_pPrevHoldEnemy = nullptr;
	CEnemy* m_pAttackTargetEnemy = nullptr;

	//�˱� ����Ʈ
	enum { SWORD_EFFECT_COUNT = 10 };
	class CSwordTrail* m_pSwordTrail[10];
	_bool m_bUseAttack1 = false;
	_bool m_bUseAttack2 = false;
	_bool m_bUseAttack3[3] = {false, false, false};
	_bool m_bUseAttack4 = false;
	_bool m_bUseAttack5[5] = { false, false, false, false, false };

	_bool m_bUseSkillA[4] = { false, false, false, false };
	_bool m_bUseSkillB = false;
	_bool m_bUseSkillC[5] = { false, false, false, false, false };

	//���Ž� ����� ����Ʈ
	CVIBuffer_Rect* m_pEvolutionSmoke = nullptr;
	CTexture* m_pSmokeDiffuse = nullptr;
	CTexture* m_pSmokeMask = nullptr;
	CShader* m_pRectShader = nullptr;
	CTransform* m_pSmokeTransform1 = nullptr;
	CTransform* m_pSmokeTransform2 = nullptr;
	CTransform* m_pSmokeTransform3 = nullptr;

	_float m_fCurrentAuraCount = 0.f;
	_float m_fCurrentAuraAcc = 0.f;

	_float m_fSmokeTimeAcc = 0.f;
	_float m_fSmokeTimeAcc2 = 0.f;
	_float m_fSmokeTimeAcc3 = 0.f;

	//�÷��̾� ��Ŭ
	class CFloorCircle* m_pFloorCircle = nullptr;

	//������
	_bool m_bBlurStart = false;
	_float m_fBlurValue = 20.f;
	_float m_fCurBlurAmount = 0.f;

	//��� ��ų ����Ʈ
	_bool m_bActiveBlueEffect = false;
	_bool m_bActiveBlueCamShake = false;
	class CSkillBlueEffect* m_pSkillBlueEffect = nullptr;

	//ǲ ����ũ ����Ʈ
	class CFootSmoke* m_pFootSmoke[2];
	_bool m_bAttack1Smoke = false;
	_bool m_bAttack4Smoke = false;
	_bool m_bAttack5Smoke = false;
	_bool m_bAttack11Smoke = false;
	_bool m_bAttack21Smoke = false;
	_bool m_bAttack31Smoke = false;
	_bool m_bAttack41Smoke = false;
	_bool m_bAttack43Smoke = false;
	_bool m_bAttack44Smoke = false;
	_bool m_bAttack45Smoke = false;
	_bool m_bAttack22Smoke = false;
	_bool m_bAttack32Smoke = false;

	class CFlower* m_pFlower = nullptr;
	_double m_TimeDelta = 0.f;

	class CEvolutionEffect* m_pEvolutionEffect = nullptr;

	//��ü �ܻ�
	enum { PREV_COUNT = 8 };
	_int m_iCurrentPrevCount = 0;
	_float m_fMotionAcc = 0.f;

	_bool m_bMotionIsUse[PREV_COUNT];
	_float m_fMotionAlphaAcc[PREV_COUNT];

	CModel* m_pPrevModel[PREV_COUNT];
	CTransform* m_pPrevTransform[PREV_COUNT];
	_double m_PrevLocalTime[PREV_COUNT];
	CTexture* m_pRainbowTexture = nullptr;

	class CFootstep* m_pFoot[10];
	CBone* LeftFoot = nullptr;
	CBone* RightFoot = nullptr;
	//Left Right
	_bool m_bFootAir[2] = { false, false };

	//�⺻���� ����
	_bool m_bAttackSound = false;
	_bool m_bSkillSound = false;
	_bool m_bDashSound = false;
	CLIP m_PrevClip = BORN;
	CLIP m_ePrevSkill = BORN;
	_bool m_bEnemyNear = false;

	//�ʻ� ����
	_bool m_bFreezeSound = false;

	_float m_fAttackVolume = 0.5f;
	_bool m_bHitSound = false;

	_bool m_bStartSound = false;
	_bool m_bWinSound = false;
	
	_bool m_bDashEffect = false;
	class CDashSprite* m_pDashSprite = nullptr;
	class CBuffHandler* m_pBuffhandler = nullptr;


	_float		m_fVoiceX, m_fVoiceY, m_fVoiceWidth, m_fVoiceHeight;
	_float4x4	m_VoiceWorldMatrix, m_ViewMatrix, m_ProjMatrix;
	CTexture* m_pTextureStart = nullptr;
	CTexture* m_pTextureEnd = nullptr;
	CShader* m_pTexShader = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;

};

END

//XMQuaternionSlerpd
