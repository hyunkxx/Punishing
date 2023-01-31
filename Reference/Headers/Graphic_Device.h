#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGraphic_Device final : public CBase
{
	DECLARE_SINGLETON(CGraphic_Device)

private:
	CGraphic_Device() = default;
	virtual ~CGraphic_Device() = default;

public:
	HRESULT Ready_Graphic_Device(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice_out, ID3D11DeviceContext** ppContext_out);
	HRESULT Clear_RenderTargetView(_float4 vClearColor);
	HRESULT Clear_DepthStencilView();
	HRESULT Present();

private:
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_RenderTargetView();
	HRESULT Ready_DepthStencilView(_uint iWinSizeX, _uint iWinSizeY);

public:
	virtual void Free() override;

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	IDXGISwapChain*				m_pSwapChain = { nullptr };

	/* pRTV & pDSV */
	ID3D11RenderTargetView*		m_pRenderTargetView = { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };

};

END