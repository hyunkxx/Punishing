#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CTexture;
class CTransform;
class CRenderer;
class CShader;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CSprite : public CGameObject
{
public:
	enum class SPRITE { NONE, BILLBOARD };
	enum class SPRITE_PLAY { ONE, LOOP };
	typedef CSprite::SPRITE SPRITE_TYPE;
	typedef CSprite::SPRITE_PLAY SPRITE_PLAY;

public:
	explicit CSprite(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CSprite(const CSprite& rhs);
	virtual ~CSprite() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual void RenderGUI() override;

public:
	void SetSpeed(float fSpeed);
	void SetupSprite(CTexture* pTexture, _float2 spriteSize, SPRITE_PLAY ePlayType);
	HRESULT SetupSpriteShaderResource(SPRITE eType);

private:
	HRESULT AddComponents();

public:
	static CSprite* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

protected:
	SPRITE_PLAY m_ePlayType = SPRITE_PLAY::LOOP;

	_bool m_bRender = true;

	_float2 m_SpriteSize;
	_float m_fSpriteAcc = 0.0f;
	_float m_fSpriteSpeed = 0.1f;
	_float m_fCurrentSpriteIndex = 0.f;

	CRenderer* m_pRenderer = nullptr;
	CTransform* m_pTransform = nullptr;
	CShader* m_pSpriteShader = nullptr;
	CTexture* m_pSpriteTexture = nullptr;
	CVIBuffer_Rect* m_pVIBufferRect = nullptr;
};

END