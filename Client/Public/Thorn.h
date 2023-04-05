#pragma once

#include "GameObject.h"
#include "Collider.h"

BEGIN(Engine)
class CRenderer;
class CTransform;
class CSphereCollider;
class CVIBuffer_Rect;
class CTexture;
class CModel;
class CShader;
class CBone;
class CModel;
class IOnCollisionEnter;
class IOnCollisionStay;
class IOnCollisionExit;
END

BEGIN(Client)

class CThorn final 
	: public CGameObject
	, public ISameObjectNoDetection
	, public IOnCollisionEnter
	, public IOnCollisionStay
	, public IOnCollisionExit
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
	void SetTimeDelta(_double TimeDelta) {
		m_FixedTimeDelta = TimeDelta;
	}

	void Reset();

	void SetType(TYPE eType) { m_eThornType = eType; }
	void SetPosition(_fvector vPos);
	void SetRotationXYZ(_float3 vAngle);
	void SetRotationToTarget(_fvector vPlayerPos);
	void SetMoveStart() { m_bMove = true; };
	void SetupScaleUpStart(_float fLength);
	void SetupScaleUpDownStart(_float fLength);
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

private:
	virtual void SameObjectNoDetection() override;
	virtual void OnCollisionEnter(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionStay(CCollider * src, CCollider * dest) override;
	virtual void OnCollisionExit(CCollider * src, CCollider * dest) override;

public:
	static CThorn* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg) override;
	virtual void Free() override;

private:
	CSphereCollider* m_pCollider = nullptr;

	TYPE m_eThornType = THORN;
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CModel* m_pModel = nullptr;
	CShader* m_pShader = nullptr;

	_bool m_bAlpha = false;
	//각도는 랜덤으로 길이는 보관
	_bool m_bRender = false;
	_bool m_bScaleFinish = false;

	_bool m_bScaleUpDown = false;	//커지면 바로 작아짐
	_bool m_bScaleUp = false;		//커지고 대기
	_bool m_bScaleSmoothUp = false; //부드럽게 커짐

	_bool m_bScaleDownFinish = false;
	_bool m_bScaleSmoothDown = false;
	_float m_fScaleAcc = 0.f;
	_float m_fLength = 1.f;		//커져야할 디폴트 길이
	const _float3 m_fPrevScale = { 0.2f, 0.2f, 0.2f };

	_float m_fWaitAcc = 0.f;
	const _float m_fWaitTime = 2.f;

	//모델의 Up방향으로 움직임 라이프 타임 5초
	_bool m_bMove = false;
	_float m_fMoveAcc = 0.f;

	//바닥 텍스쳐
	CTransform* m_pFloorEffectTransform = nullptr;
	CVIBuffer_Rect* m_pFloorEffectBuffer = nullptr;
	CShader* m_pFloorShader = nullptr;

	CTexture* m_pFloorEffectDiffuse = nullptr;
	CTexture* m_pFloorEffectMask = nullptr;

	CModel* m_pFloorGard = nullptr;
	CTransform* m_pFloorGardTransform = nullptr;
	CTexture* m_pFloorGardTexture = nullptr;

	_double m_FixedTimeDelta = 0.f;
	_float m_fTimeAcc = 0.f;
	_bool m_bGardRender = false;
	_bool m_bFloorRender = false;
	_float m_fFloorAlphaAcc = 0.f;

	//미사일 궤적
	CVIBuffer_Rect* m_pTrailEffectBuffer = nullptr;
	CTransform* m_pTrailTransform = nullptr;
	CTexture* m_pTrailTexture = nullptr;
	CShader* m_pTrailEffectShader = nullptr;
	_float3 vTrailScale = { 0.25f, 0.25f, 0.25f };
	_float m_vTrailAcc = 0.f;
};

END