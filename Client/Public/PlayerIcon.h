#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CPlayerIcon final : public CGameObject
{
public:
	enum { NUM_SIZE_X = 38 };

protected:
	explicit CPlayerIcon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CPlayerIcon(const CPlayerIcon& rhs);
	virtual ~CPlayerIcon() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void StartShake() { m_isShake = true; m_fShakeAcc = 0.f; }
	void SetComboPerTime(_float fCurTime) { m_fComboCurTime = fCurTime; }
	void SetCombo(_int iComboCount) { m_iCombo = iComboCount; }
	void SetRender(_bool value) { m_bRender = value; }
	void SetupPlayer(class CCharacter* pPlayer) { m_pPlayer = pPlayer; }
	CTexture* ComputeComboToTexture(int iIndex);

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CPlayerIcon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float		m_fTargetAngle;
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_ViewMatrix, m_ProjMatrix;

	//버튼별 위치
	_float4x4	m_DashBackMatrix;
	_float4x4	m_AttackMatrix, m_DashMatrix;
	_float4x4	m_TargetMatrix;

	//콤보 숫자
	_float		m_fComboNumX, m_fComboNumY, m_fComboNumWidth, m_fComboNumHeight;
	_float		m_fOriginComboNumX = 150.f;
	_float4x4	m_ComboNumberMatrix[4];

	//콤보 이미지
	_float		m_fComboImageX, m_fComboImageWidth, m_fComboImageHeight;
	_float4x4	m_ComboMatrix;

	//콤보 게이지
	_float		m_fComboGageX, m_fComboGageY, m_fComboGageWidth, m_fComboGageHeight;
	_float4x4	m_ComboGageMatrix;

	_float		m_fTargetX, m_fTargetY, m_fTargetWidth, m_fTargetHeight;
	_float		m_fGageX, m_fGageY, m_fGageWidth, m_fGageHeight;

	//변신 게이지
	_float		m_fEvolutionGageX, m_fEvolutionGageY, m_fEvolutionGageWidth, m_fEvolutionGageHeight;
	_float4x4	m_EvolutionMatrix;

	//변신 버튼
	_float		m_fEvolutionButtonX, m_fEvolutionButtonY, m_fEvolutionButtonXWidth, m_fEvolutionButtonXHeight;
	_float4x4	m_EvolutionButtonMatrix, m_EvolutionBackMatrix;

	_float4x4	m_DashRotMatrix,m_EvolutionRotMatrix;
private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	//백그라운드 아이콘
	CVIBuffer_Rect* m_pBackVIBuffer = { nullptr };
	CTexture*		m_pBackTexture = { nullptr };

	//아이콘
	CVIBuffer_Rect* m_pAttackVIBuffer = { nullptr };
	CTexture*		m_pAttackTexture = { nullptr };

	CVIBuffer_Rect* m_pDashVIBuffer = { nullptr };
	CTexture*		m_pDashTexture = { nullptr };

	CVIBuffer_Rect* m_pEvolutionVIBuffer = { nullptr };
	CTexture*		m_pEvolutionTexture = { nullptr };

	//타겟 이미지
	_bool m_bTargetImageRender = false;
	CVIBuffer_Rect* m_pTargetVIBuffer = { nullptr };
	CTexture*		m_pTargetTexture = nullptr;


	//콤보 이미지
	CVIBuffer_Rect* m_pComboNumberBuffer = { nullptr };
	CTexture*		m_pComboNumberTexture[10] = { nullptr };

	CVIBuffer_Rect* m_pComboBuffer = { nullptr };
	CTexture*		m_pComboTexture = { nullptr };

	CVIBuffer_Rect* m_pComboGageBuffer = { nullptr };
	CTexture*		m_pComboGageTexture = { nullptr };

	//변신 게이지
	CVIBuffer_Rect* m_pEvolutionGageBuffer = { nullptr };
	CTexture*		m_pEvolutionGageBackTexture = { nullptr };
	CTexture*		m_pEvolutionGageFrontTexture = { nullptr };

	CTexture*		m_pRotationTexture = { nullptr };

	//변신 버튼
	CVIBuffer_Rect* m_pEvolutionButtonBuffer = { nullptr };
	CTexture*		m_pEvolutionButtonTexture = { nullptr };

private:
	_bool m_bRender = true;
	class CCharacter* m_pPlayer = nullptr;

	_bool m_bComboRender = true;
	_int m_iCombo = 0;

	//콤보 게이지
	_float m_fFill = 1.f;
	_float m_fComboCurTime = 0.0f;
	const _float m_fComboTimeOut = 3.5f;

	//분노 게이지
	_float m_fEvolutionGageFill = 1.f;

	//쉐이크
	_float m_fShakeX = 0.f;//이전위치 기록
	_float m_fShakeY = 0.f;//이전위치 기록
	_bool m_isShake = false;
	_float m_fShakeAcc = 0.f;
	const _float m_fShakeTimeOut = 2.5f;

	_bool m_bMaxAlpha = false;
	_float m_fEvolutionAlpha = 1.f;

	//타겟 락인 이미지 0~138
	_int m_iCurrentIndex = 0;
	_float m_fImageAcc = 0.f;

	_float m_fAngleEvolution = 0.f;

};

END