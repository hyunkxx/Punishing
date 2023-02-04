#pragma once

#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTransform;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Tool)

class CTerrain final : public CGameObject
{
protected:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override; 
	virtual void RenderGUI() override;

public:
	_bool Terrain_Picking();

private:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CShader*			m_pShader = { nullptr };
	CTexture*			m_pTexture = { nullptr };
	CRenderer*			m_pRenderer = { nullptr };
	CTransform*			m_pTransform = { nullptr };
	CVIBuffer_Terrain*	m_pVIBuffer = { nullptr };

private:
	_bool m_IsPick = { false };
	_float m_fDistance = { 1000.f };
	_float4 origin, dir, vCam;
	_float4 pickPos;
	_float3 m_v1, m_v2, m_v3;
};

END