#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSkillBase final : public CGameObject
{
public:
	enum class TYPE { RED, BLUE, YELLOW, INVALID };
	enum class KEY { A, S, D, F, Q, W, E, R };

	typedef struct tagSkillChainData
	{
		_int iChainCount;
		_int iLeftIndex;
		_int iRightIndex;
	}CHAIN_DATA;

	typedef struct tagSkillInfo
	{
		_int iChainCount;
		TYPE eType;
	}SKILL_INFO;

private:
	CSkillBase(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillBase(const CSkillBase& rhs);
	virtual ~CSkillBase() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	static _uint GetSkillID() { return s_iID; }
	void SetRender(_bool value) { m_bRender = value; }
	void SetAllgn(_bool value) { m_bAlign = value; }
	void SetDestroyWait() { m_isDestroyWait = true; }

public:
	_bool IsSameType(TYPE eType) { return m_eType == eType; }
	TYPE GetSkillType() const { return m_eType; }
	void SetIndex(_uint iIndex);

protected:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CSkillBase* Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext);
	virtual CGameObject* Clone(void * pArg = nullptr) override;
	virtual void Free() override;

protected:
	_float		m_fX, m_fY, m_fWidth, m_fHeight;
	_float4x4	m_WorldMatrix, m_ViewMatrix, m_ProjMatrix;
	_float4x4	m_KeyWorldMatrix;
	_float4x4	m_UsedMatrix;

protected:
	CRenderer*		m_pRenderer = { nullptr };
	CShader*		m_pShader = { nullptr };
	CVIBuffer_Rect* m_pVIBuffer = { nullptr };
	CTexture*		m_pTexture = { nullptr };

	CVIBuffer_Rect* m_pKeyBackgroundBuffer = { nullptr };
	CTexture*		m_pKeyTexture[8];

	CVIBuffer_Rect* m_pWhiteBuffer = { nullptr };
	CTexture*		m_pTextureWhite = { nullptr };

	CVIBuffer_Rect* m_pUseBuffer = { nullptr };
	CTexture*		m_pTextureUse = { nullptr };
protected:
	static _uint s_iID;
	_bool m_bRender = true;

	_uint m_iIndex = 0;
	_float m_fGoalX;

	_bool m_bAlign = false;
	_bool m_bAllgnEffectFinish = false;

	_float m_fEffectAlpha = 1.f;

	TYPE m_eType = TYPE::INVALID;

	_bool m_isDestroyWait = false;
	_float m_fDestroyWaitTimer = 1.f;
	const _float m_fDestroyWaitTimeOut = 0.f;
};

END