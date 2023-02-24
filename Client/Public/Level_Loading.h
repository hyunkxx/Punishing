#pragma once

#include "Level.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;
public:
	virtual HRESULT Initialize(LEVEL_ID eNextLevel);
	virtual void Tick(_double TimeDelta) override;

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL_ID eNextLevel);
	virtual void Free() override;

private:
	LEVEL_ID		m_eNextLevel = { LEVEL_END };
	class CLoader*	m_pLoader = { nullptr };

	class CGameObject* m_pBackground = nullptr;
};

END