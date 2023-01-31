#pragma once

#include "Level.h"

BEGIN(Tool)

class CMapTool final : public CLevel
{
private:
	CMapTool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CMapTool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

private:
	HRESULT Ready_Layer_BackGround(const _tchar* pLayerTag);

	HRESULT Ready_Layre_Camera(const _tchar * pLayerTag);

public:
	static CMapTool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END