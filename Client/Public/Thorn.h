#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CModel;
class CShader;
class CBone;
class CModel;
END

BEGIN(Client)

class CThorn final : public CGameObject
{
public:
	//가시형태, 미사일형태
	enum TYPE { THORN, MISSILE };
public:
	typedef struct tagOwner
	{
		CModel* pModel;
		CTransform* pTransform;
		const CBone* pWeaponCase;
	}OWNER_DESC;

private:
	CThorn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CThorn() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

	virtual _float GetLengthFromCamera() override;
public://초기화, 스케일, 회전
	void Reset();

	void SetType(TYPE eType) { m_eThornType = eType; }
	void SetPosition(_fvector vPos);
	void SetRotationXYZ(_float3 vAngle);
	void SetRotationToTarget(_fvector vPlayerPos);
	void SetMoveStart() { m_bMove = true; };
	void SetupScaleUpStart(_float fLength);
	void SetupScaleSmoothUpStart(_float fLength);
	void SetupScaleSmoothDownStart();
	_bool ScaleUpProcess(_double TimeDelta);
	_bool ScaleUpSmoothProcess(_double TimeDelta);
	_bool ScaleDownSmoothProcess(_double TimeDelta);

	_bool MoveProcess(_double TimeDelta);

	void SetRender(_bool value) { m_bRender = value; }
	_bool IsRender() { return m_bRender; }
	_bool IsScaleFinish() { return m_bScaleFinish; };

private:
	HRESULT AddComponents();
	HRESULT SetupShaderResources();

public:
	static CThorn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	TYPE m_eThornType = THORN;
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CModel* m_pModel = nullptr;
	CShader* m_pShader = nullptr;

	_bool m_bAlpha = false;
	//각도는 랜덤으로 길이는 보관
	_bool m_bRender = false;
	_bool m_bScaleFinish = false;
	_bool m_bScaleUp = false;
	_bool m_bScaleSmoothUp = false;

	_bool m_bScaleDownFinish = false;
	_bool m_bScaleSmoothDown = false;
	_float m_fScaleAcc = 0.f;
	_float m_fLength = 1.f;		//커져야할 디폴트 길이
	const _float3 m_fPrevScale = { 0.f, 0.f, 0.f};

	_float m_fWaitAcc = 0.f;
	const _float m_fWaitTime = 2.f;

	//모델의 Up방향으로 움직임 라이프 타임 5초
	_bool m_bMove = false;
	_float m_fMoveAcc = 0.f;
};

END