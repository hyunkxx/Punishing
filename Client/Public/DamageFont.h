#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CDamageFont final : public CGameObject
{
protected:
	CDamageFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDamageFont(const CDamageFont& rhs);
	virtual ~CDamageFont() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetRender(_bool value) { m_bRender = value; }
	void SetupPlayer(class CCharacter* pPlayer) { m_pPlayer = pPlayer; }
	CTexture* ComputeComboToTexture(int iIndex);

	void SetPosition();

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CDamageFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	//데미지
	_float4x4	m_ViewMatrix, m_ProjMatrix;
	_float		m_fDamageX, m_fDamageY, m_fDamageWidth, m_fDamageHeight;
	_float		m_fOriginDamageX = 150.f;
	_float4x4	m_DamageMatrix[4];

private:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };

	//데미지 이미지
	CVIBuffer_Rect* m_pDamageBuffer = { nullptr };
	CTexture*		m_pDamageTexture[10] = { nullptr };

private:
	string m_strCombo = "123";
	_bool m_bRender = true;
	class CCharacter* m_pPlayer = nullptr;

};

END