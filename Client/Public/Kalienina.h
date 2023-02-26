#pragma once

#include "GameObject.h"
#include "Animation.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
class CTimer;
END

BEGIN(Client)

class CKalienina final : public CGameObject
{
public:
	enum CLIP
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


	};

private:
	CKalienina(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CKalienina(const CKalienina& rhs);
	virtual ~CKalienina() = default;

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
	void RigthRotation(_double TimeDelta);
	void LeftRotation(_double TimeDelta);
	void Movement(_double TimeDelta);
	void InputWASD(_double TimeDelta);

private:
	void LerpTest(_double TimeDelta);
	void KeyInput(_double TimeDelta);

	void Idle();
	void MoveForward(_double TimeDelta);
	void MoveBackward(_double TimeDelta);
	void MoveRight(_double TimeDelta);
	void MoveLeft(_double TimeDelta);
	void MoveStop();
	void Attack();

public:
	static CKalienina* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* mRenderer = nullptr;
	CTransform* mTransform = nullptr;
	CModel* mModel = nullptr;
	CShader* mShader = nullptr;

private: //레이어 삭제시 삭제됨
	class CSleeve* m_pWeapon;

	CAnimation::ANIMATION_DESC ANIM_DESC;

private: // camera
	CTransform* mCameraSocketTransform = nullptr;
	CTransform* mCameraPosition = nullptr;

private: // Command
	_uint m_iAttackCount = 4;
	_uint m_iCurAttackCount = 0;
	
	_bool m_bCombo = false;
	const _float m_fComboTimerMax = 1.f;

	_bool m_bMoveable = true;
	_bool m_bAttackable = true;

	_int m_iWASDCount = 0;
	_bool m_bMove = false;
	_bool m_bRun = false;
	_bool m_bDashReady = false;
	_bool m_bOnTerrain = true;

	_float m_fMoveSpeed = 5.f;
	_float m_fRotationSpeed = 240.f;
};



END