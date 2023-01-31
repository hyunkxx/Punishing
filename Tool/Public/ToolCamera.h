#pragma once

#include "Camera.h"

BEGIN(Tool)

class CToolCamera final : public CCamera
{
private:
	CToolCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CToolCamera(const CToolCamera& rhs);
	virtual ~CToolCamera() = default;

public:
	virtual	HRESULT Initialize_Prototype() override;
	virtual	HRESULT Initialize(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void LateTick(_double TimeDelta) override;
	virtual HRESULT Render();

public:
	HRESULT Add_Components();
	HRESULT Setup_ShaderResources();

public:
	static CToolCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	bool m_isRotationLock = { true };

};

END