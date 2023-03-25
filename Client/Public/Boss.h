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

END

BEGIN(Client)
class CCharacter;

class CBoss final : public CEnemy
{
public:
	enum BOSS_CLIP
	{
		ATK_FOWARD,
		ATK_BACK,
		ATK3,
		ATK4,
		ATK5,//����
		ATK11,
		ATK12,//��
		ATK13,
		UNKNOWN1,
		UNKNOWN2,
		UNKNOWN3,
		UNKNOWN4,
		UNKNOWN5,
		UNKNOWN6,
		BORN,
		DEATH,
		RECORDED,
		STAND1,//2������
		STAND2,//1������
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

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public://������
	_bool IsBurrow() const { return m_bBrrow; }
	_bool IsSpawned() const { return m_bSpawn; }

public://��ų
	void Spawn();
	void MoveForward(_double TimeDelta);
	void MoveBackword(_double TimeDelta);
	void LineSkill(_double TimeDelta, _int iIndex);
	void Missile1(_double TimeDelta);

public: //�÷��̾� ����
	_float GetLengthFromPlayer() const;
	_bool CloseToPlayer() { return GetLengthFromPlayer() <= m_fNearCheckRange; };
	_bool LookTarget(_double TimeDelta, _float fRotationSpeed);

private://�ִϸ��̼� ����
	void SetupState(BOSS_CLIP eClip, CAnimation::TYPE eAnimType, _bool bLerp) 
	{ 
		m_eAnimState.eCurAnimationClip = eClip;
		m_eAnimState.eAnimType = eAnimType;
		m_eAnimState.bLerp = bLerp;
	}


	void AnimationController(_double TimeDelta);

private://�ݶ��̴� ����
	void SetupColliders();

public:
	virtual _float4 GetPosition() override;
	virtual _float GetLengthFromCamera() override;

	//Collider
	virtual class CCollider* GetBodyCollider() override { return collider; }
	virtual class CCollider* GetWeaponCollider() override  { return m_pWeaponCollider; }
	virtual class CCollider* GetOverlapCollider() override  { return m_pOverlapCollider; }

	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;

public:
	static CBoss* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	ANIM_STATE m_eAnimState;
	
	_bool m_bRotationFinish = false;
	_bool m_bAttackable = false;
	_int m_iAttackCount = 0;

	_bool m_bSpawn = false;
	const _float m_fNearCheckRange = 10.f;	 // �÷��̾�� ������� üũ 10

	_bool m_bBrrow = false;

	//���� 3Ÿ ����
	_bool m_bLineAttack = false;

	class CThorn* m_pThorn1[25][3];
	class CThorn* m_pThorn2[25][3];

	_float3 vThornSpawnPos = { 0.f, 0.f, 0.f };

	_int m_iCurrentOneIndex[3] = {0, 0, 0};
	_int m_iCurrentTwoIndex[3] = {0, 0, 0};

	_int m_iEraseIndex[3] = { 0, 0, 0 };

	_bool m_bUseLineSkill = false;
	_bool m_bLineSkillStart[3] = { false, false, false };
	_bool m_bLineSkillErase[3] = { false, false, false };
	
	float m_fEraseAcc[3] = { 0.f, 0.f, 0.f };
	const float m_fEraseTime = 0.05f;

	_bool m_bCheckDir[3] = { false, false, false };
	_float3 m_vLineOneDir[3];

	_bool m_bEvolution = false;

	//���� 3Ÿ ���� - �� 3ȸ�� ����� ��������
	_int m_iCurrentLine = 0;
	_float m_fLineUpAcc = 0.f;
	const _float m_fLineUpTime = 0.5f;
	
	//���� ��ü ���� �ø������� Ʈ������
	CTransform* m_pBodyTransform = nullptr;

	_bool m_bMoveForward= false;
	_bool m_bMoveBackward= false;

	//�ڷ� �̵� ATK_BACK
	_bool m_bBack = false;
	_float3 m_vPrevPos = { 0.f,0.f,0.f };
	_float3 m_vPrevLook = { 0.f, 0.f, 0.f };

	_float m_fAttackAcc = 2.f;
	const _float m_fAttackAccTime = 2.f;


	//�̻��� 9�� \ | / �������� ������
	class CThorn* m_pThornMissileLeft[3];
	class CThorn* m_pThornMissileMiddle[3];
	class CThorn* m_pThornMissileRight[3];

	_bool m_bUseMissile1 = false;
	_bool m_bMissileStart = false;
};

END