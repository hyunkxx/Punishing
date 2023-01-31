#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);

public:
	virtual void Free() override;

protected:
	ID3D11Device*			m_pDevice;
	ID3D11DeviceContext*	m_pContext;

};

END