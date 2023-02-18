#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
END

BEGIN(Client)

class CKalienina : public CGameObject
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
		RUN_END,
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
	HRESULT AddComponents();
	HRESULT SetupShaderResources();
	void Movement(_double TimeDelta);
	void AnimationControl();

public:
	static CKalienina* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CRenderer* mRenderer = nullptr;
	CTransform* mTransform = nullptr;
	CModel* mModel = nullptr;
	CShader* mShader = nullptr;

	CLIP eCurrentClip = STAND1;
};



END