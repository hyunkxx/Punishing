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
	enum PRE_RENDERTARGET { MAIN, BLOOM, DISTORTION, DEPTH, SHADOW_DEPTH, PRE_TARGET_MAX };
	enum POST_RENDERTARGET { BACK_BUFFER, CURRENT_BUFFER, SHADOW_BUFFER , MAIN_SHADOW, XBLUR_BUFFER, YBLUR_BUFFER, POST_DISTORTION, FINAL_BUFFER, POST_TARGET_MAX };

public:
	HRESULT Ready_Graphic_Device(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device** ppDevice_out, ID3D11DeviceContext** ppContext_out);
	HRESULT Clear_RenderTargetView(_float4 vClearColor);
	HRESULT Clear_DepthStencilView();
	HRESULT Present();

public://Pre Rendertarget
	HRESULT SetPreRenderTargets();
	HRESULT SetPreRenderTarget(PRE_RENDERTARGET eTarget);
	HRESULT Clear_PreRenderTargetViews(_float4 vClearColor);
	HRESULT Clear_PreRenderTargetViews(PRE_RENDERTARGET eTarget, _float4 vClearColor);
	ID3D11RenderTargetView* GetRenderTarget(PRE_RENDERTARGET eTarget) { return m_pPreRenderTargetViews[eTarget]; };
	ID3D11ShaderResourceView* GetShaderResourceView(PRE_RENDERTARGET eTarget) { return m_pPreShaderResourceViews[eTarget]; };

public://Post Rendertarget
	HRESULT Clear_RenderTargetView(POST_RENDERTARGET eTarget, _float4 vClearColor);
	void	SetRenderTarget(POST_RENDERTARGET eTarget);
	ID3D11RenderTargetView* GetRenderTarget(POST_RENDERTARGET eTarget) { return m_pRenderTargetViews[eTarget]; };
	ID3D11ShaderResourceView* GetShaderResourceView(POST_RENDERTARGET eTarget) { return m_pShaderResourceViews[eTarget]; };

private:
	HRESULT Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY);
	HRESULT Ready_RenderTargetView();
	HRESULT Ready_PreRenderTargetViews();
	HRESULT Ready_PostRenderTargetViews();
	HRESULT Ready_DepthStencilView(_uint iWinSizeX, _uint iWinSizeY);

public:
	virtual void Free() override;

private:
	HWND m_hWnd = 0;
	GRAPHIC_DESC::WIN_MODE m_eMode;
	_uint m_iWinSizeX = 0;
	_uint m_iWinSizeY = 0;

private:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

	IDXGISwapChain*				m_pSwapChain = { nullptr };

	/* pRTV & pDSV */
	ID3D11RenderTargetView*		m_pRenderTargetView = { nullptr };
	ID3D11DepthStencilView*		m_pDepthStencilView = { nullptr };
		
private:
	//이펙트 적용전 그리기위한 렌더타겟들
	ID3D11Texture2D*			m_pPreBufferTextures[PRE_TARGET_MAX];
	ID3D11RenderTargetView*		m_pPreRenderTargetViews[PRE_TARGET_MAX];
	ID3D11ShaderResourceView*	m_pPreShaderResourceViews[PRE_TARGET_MAX];

	//전체화면 출력 렌더타겟들
	ID3D11Texture2D*			m_pBufferTextures[POST_TARGET_MAX];
	ID3D11RenderTargetView*		m_pRenderTargetViews[POST_TARGET_MAX];
	ID3D11ShaderResourceView*	m_pShaderResourceViews[POST_TARGET_MAX];

};

END