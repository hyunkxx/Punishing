#pragma once

#include "Base.h"

BEGIN(Client)

class CImGUIManager final : public CBase
{
	DECLARE_SINGLETON(CImGUIManager)
public:
	CImGUIManager();
	~CImGUIManager();

public:
	void Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void NewFrame();
	void RenderDrawData();
	void Render();
	void Shutdown();

public:
	virtual void Free() override;

private:
	ID3D11Device*		 m_pDevice{ nullptr };
	ID3D11DeviceContext* m_pContext{ nullptr };

};

END