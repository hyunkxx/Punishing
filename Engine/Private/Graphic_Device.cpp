#include "..\Public\Graphic_Device.h"

IMPLEMENT_SINGLETON(CGraphic_Device)

HRESULT CGraphic_Device::Ready_Graphic_Device(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY, ID3D11Device ** ppDevice_out, ID3D11DeviceContext ** ppContext_out)
{
	_uint iFlag = 0;

#ifdef _DEBUG
	iFlag = D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_FEATURE_LEVEL FeatureLevel;

	if (FAILED(D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, iFlag, nullptr, 0, D3D11_SDK_VERSION, &m_pDevice, &FeatureLevel, &m_pContext)))
		return E_FAIL;

	if (FAILED(Ready_SwapChain(hWnd, eWinMode, iWinSizeX, iWinSizeY)))
		return E_FAIL;

	if (FAILED(Ready_RenderTargetView()))
		return E_FAIL;

	if (FAILED(Ready_PreRenderTargetViews()))
		return E_FAIL;

	if (FAILED(Ready_PostRenderTargetViews()))
		return E_FAIL;

	if (FAILED(Ready_DepthStencilView(iWinSizeX, iWinSizeY)))
		return E_FAIL;

	//장치에 바인딩 할 렌더타겟들과 뎁스,스텐실뷰를 세팅한다.
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);

	D3D11_VIEWPORT viewPortDesc;
	ZeroMemory(&viewPortDesc, sizeof D3D11_VIEWPORT);
	viewPortDesc.TopLeftX = 0;
	viewPortDesc.TopLeftY = 0;
	viewPortDesc.Width = (_float)iWinSizeX;
	viewPortDesc.Height = (_float)iWinSizeY;
	viewPortDesc.MinDepth = 0.f;
	viewPortDesc.MaxDepth = 1.f;

	m_pContext->RSSetViewports(1, &viewPortDesc);

	*ppDevice_out = m_pDevice;
	*ppContext_out = m_pContext;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_RenderTargetView(_float4 vClearColor)
{
	if (nullptr == m_pContext)
		return E_FAIL;
	
	m_pContext->ClearRenderTargetView(m_pRenderTargetView, (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_DepthStencilView()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D10_CLEAR_STENCIL, 1.f, 0);

	return S_OK;
}

HRESULT CGraphic_Device::Present()
{
	if (nullptr == m_pSwapChain)
		return E_FAIL;

	return m_pSwapChain->Present(0, 0);
}

HRESULT CGraphic_Device::SetPreRenderTargets()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	m_pContext->OMSetRenderTargets(PRE_TARGET_MAX, m_pPreRenderTargetViews, m_pDepthStencilView);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_PreRenderTargetViews(_float4 vClearColor)
{
	if (nullptr == m_pContext)
		return E_FAIL;

	for (int i = 0; i < PRE_TARGET_MAX; ++i)
		m_pContext->ClearRenderTargetView(m_pPreRenderTargetViews[i], (_float*)&vClearColor);

	return S_OK;
}

HRESULT CGraphic_Device::Clear_RenderTargetView(POST_RENDERTARGET eTarget, _float4 vClearColor)
{
	if (nullptr == m_pContext)
		return E_FAIL;

	if (eTarget == POST_RENDERTARGET::BACK_BUFFER)
		m_pContext->ClearRenderTargetView(m_pRenderTargetView, (_float*)&vClearColor);
	else
		m_pContext->ClearRenderTargetView(m_pRenderTargetViews[eTarget], (_float*)&vClearColor);
	
	return S_OK;
}

void CGraphic_Device::SetRenderTarget(POST_RENDERTARGET eTarget)
{
	if(eTarget == POST_RENDERTARGET::BACK_BUFFER)
		m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	else
		m_pContext->OMSetRenderTargets(1, &m_pRenderTargetViews[eTarget], m_pDepthStencilView);

	return;
}

HRESULT CGraphic_Device::Ready_SwapChain(HWND hWnd, GRAPHIC_DESC::WIN_MODE eWinMode, _uint iWinSizeX, _uint iWinSizeY)
{
	m_hWnd = hWnd;
	m_eMode = eWinMode;
	m_iWinSizeX = iWinSizeX;
	m_iWinSizeY = iWinSizeY;

	IDXGIDevice*			pDevice = nullptr;
	m_pDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDevice);

	IDXGIAdapter*			pAdapter = nullptr;
	pDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pAdapter);

	IDXGIFactory*			pFactory = nullptr;
	pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pFactory);

	/* 스왑체인을 생성한다. = 텍스쳐를 생성하는 행위 + 스왑하는 형태  */
	DXGI_SWAP_CHAIN_DESC		SwapChain;
	ZeroMemory(&SwapChain, sizeof(DXGI_SWAP_CHAIN_DESC));

	/*텍스쳐(백버퍼)를 생성*/
	SwapChain.BufferDesc.Width = iWinSizeX;
	SwapChain.BufferDesc.Height = iWinSizeY;
	SwapChain.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	SwapChain.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChain.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	SwapChain.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChain.BufferCount = 1;

	/* 스왑하는 형태 */
	SwapChain.BufferDesc.RefreshRate.Numerator = 60;
	SwapChain.BufferDesc.RefreshRate.Denominator = 1;
	SwapChain.SampleDesc.Quality = 0;
	SwapChain.SampleDesc.Count = 1;
	SwapChain.OutputWindow = hWnd;
	SwapChain.Windowed = eWinMode;
	SwapChain.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	/* 백버퍼라는 텍스쳐를 생성 */
	if (FAILED(pFactory->CreateSwapChain(m_pDevice, &SwapChain, &m_pSwapChain)))
		return E_FAIL;

	Safe_Release(pFactory);
	Safe_Release(pAdapter);
	Safe_Release(pDevice);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_RenderTargetView()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D* pBackBufferTexture = nullptr;
	if (FAILED(m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&pBackBufferTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateRenderTargetView(pBackBufferTexture, nullptr, &m_pRenderTargetView)))
		return E_FAIL;

	Safe_Release(pBackBufferTexture);

	return S_OK;
}

HRESULT CGraphic_Device::Ready_PreRenderTargetViews()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	for (int i = 0; i < PRE_TARGET_MAX; ++i)
	{
		ID3D11Texture2D* pBufferTexture = nullptr;

		D3D11_TEXTURE2D_DESC textureDesc;
		HRESULT result;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		// RTT 디스크립션을 초기화
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// RTT 디스크립션을 세팅.
		textureDesc.Width = m_iWinSizeX;
		textureDesc.Height = m_iWinSizeY;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// 렌더 타겟 뷰에 대한 디스크립션을 설정
		result = m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pPreBufferTextures[i]);
		if (FAILED(result))
			return E_FAIL;

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// 렌더 타겟 뷰를 생성
		result = m_pDevice->CreateRenderTargetView(m_pPreBufferTextures[i], &renderTargetViewDesc, &m_pPreRenderTargetViews[i]);
		if (FAILED(result))
			return E_FAIL;

		// 셰이더 리소스 뷰에 대한 디스크립션을 설정
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// 셰이더 리소스 뷰를 생성
		result = m_pDevice->CreateShaderResourceView(m_pPreBufferTextures[i], &shaderResourceViewDesc, &m_pPreShaderResourceViews[i]);
		if (FAILED(result))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGraphic_Device::Ready_PostRenderTargetViews()
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	for (int i = 0; i < POST_TARGET_MAX; ++i)
	{
		ID3D11Texture2D* pBufferTexture = nullptr;

		D3D11_TEXTURE2D_DESC textureDesc;
		HRESULT result;
		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;

		// RTT 디스크립션을 초기화
		ZeroMemory(&textureDesc, sizeof(textureDesc));

		// RTT 디스크립션을 세팅.
		textureDesc.Width = m_iWinSizeX;
		textureDesc.Height = m_iWinSizeY;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = 0;

		// 렌더 타겟 뷰에 대한 디스크립션을 설정
		result = m_pDevice->CreateTexture2D(&textureDesc, NULL, &m_pBufferTextures[i]);
		if (FAILED(result))
			return E_FAIL;

		renderTargetViewDesc.Format = textureDesc.Format;
		renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
		renderTargetViewDesc.Texture2D.MipSlice = 0;

		// 렌더 타겟 뷰를 생성
		result = m_pDevice->CreateRenderTargetView(m_pBufferTextures[i], &renderTargetViewDesc, &m_pRenderTargetViews[i]);
		if (FAILED(result))
			return E_FAIL;

		// 셰이더 리소스 뷰에 대한 디스크립션을 설정
		shaderResourceViewDesc.Format = textureDesc.Format;
		shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		shaderResourceViewDesc.Texture2D.MipLevels = 1;

		// 셰이더 리소스 뷰를 생성
		result = m_pDevice->CreateShaderResourceView(m_pBufferTextures[i], &shaderResourceViewDesc, &m_pShaderResourceViews[i]);
		if (FAILED(result))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CGraphic_Device::Ready_DepthStencilView(_uint iWinSizeX, _uint iWinSizeY)
{
	if (nullptr == m_pDevice)
		return E_FAIL;

	ID3D11Texture2D*		pDepthStencilTexture = nullptr;

	D3D11_TEXTURE2D_DESC	TextureDesc;
	ZeroMemory(&TextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	TextureDesc.Width = iWinSizeX;
	TextureDesc.Height = iWinSizeY;
	TextureDesc.MipLevels = 1;
	TextureDesc.ArraySize = 1;
	TextureDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;

	TextureDesc.SampleDesc.Quality = 0;
	TextureDesc.SampleDesc.Count = 1;

	TextureDesc.Usage = D3D11_USAGE_DEFAULT;
	TextureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	TextureDesc.CPUAccessFlags = 0;
	TextureDesc.MiscFlags = 0;

	/* RenderTarget ShaderResource DepthStencil */
	if (FAILED(m_pDevice->CreateTexture2D(&TextureDesc, nullptr, &pDepthStencilTexture)))
		return E_FAIL;

	if (FAILED(m_pDevice->CreateDepthStencilView(pDepthStencilTexture, nullptr, &m_pDepthStencilView)))
		return E_FAIL;

	Safe_Release(pDepthStencilTexture);

	return S_OK;
}

void CGraphic_Device::Free()
{
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDepthStencilView);
	Safe_Release(m_pRenderTargetView);
	Safe_Release(m_pContext);

	//#if defined(DEBUG) || defined(_DEBUG)
	//	ID3D11Debug* d3dDebug;
	//	HRESULT hr = m_pDevice->QueryInterface(__uuidof(ID3D11Debug), reinterpret_cast<void**>(&d3dDebug));
	//	if (SUCCEEDED(hr))
	//	{
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//
	//		hr = d3dDebug->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
	//
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//		OutputDebugStringW(L"                                                                    D3D11 Live Object ref Count Checker END \r ");
	//		OutputDebugStringW(L"----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- \r ");
	//	}
	//	if (d3dDebug != nullptr)            d3dDebug->Release();
	//#endif


	Safe_Release(m_pDevice);
}
