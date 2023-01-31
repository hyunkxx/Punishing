#pragma once

#include "Base.h"

BEGIN(Tool)

class CGUIManager : public CBase
{
	DECLARE_SINGLETON(CGUIManager)

private:
	explicit CGUIManager();
	virtual ~CGUIManager() = default;

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