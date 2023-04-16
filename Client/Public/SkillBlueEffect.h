#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSkillBlueEffect final : public CGameObject
{
public:
	explicit CSkillBlueEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSkillBlueEffect(const CSkillBlueEffect& rhs);
	virtual ~CSkillBlueEffect() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetupPosition();

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();
	
public:
	static CSkillBlueEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CVIBuffer_Rect* m_pVIBuffer = nullptr;
	CShader* m_pShader = nullptr;
	CTexture* m_pDiffuseTexture = nullptr;
	CTexture* m_pMaskTexture = nullptr;
private:
	class CCharacter* m_pOwner = nullptr;
	CTransform* m_pPlayerTransform = nullptr;

	_bool m_bRender = false;
	_float4x4 m_CalcMatrix; // 계산된 메트릭스

	enum { THORN_COUNT = 12};
	class CThorn* m_pThorn[THORN_COUNT];
	_bool m_bScaleDown[THORN_COUNT];
	_float m_fScaleDownWait[THORN_COUNT];
	

	_float m_fAlphaAcc = 2.f;

};

END