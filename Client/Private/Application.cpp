#include "pch.h"
#include "..\Public\Application.h"

#include "ApplicationManager.h"
#include "StageCollisionManager.h"
#include "GameInstance.h"
#include "ImGUIManager.h"

#include "Level_Loading.h"

#include "BackGround.h"
#include "DynamicCamera.h"
#include "Wall.h"

CApplication::CApplication()
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGUIManager { CImGUIManager::GetInstance() }
	, m_pStageManager{ CStageCollisionManager::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CApplication::Initialize()
{
	GRAPHIC_DESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GRAPHIC_DESC);
	GraphicDesc.eMode = GRAPHIC_DESC::MODE_WINDOW;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInstance = g_hInst;
	
	if (FAILED(m_pGameInstance->Engine_Initialize(GraphicDesc, LEVEL_END, &m_pDevice, &m_pContext)))
		return E_FAIL;

	m_pGUIManager->Initialize(m_pDevice, m_pContext);

	if (FAILED(InitializeManager()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Static_Component()))
		return E_FAIL;

	if (FAILED(Ready_Prototype_Static_GameObject()))
		return E_FAIL;

	if (FAILED(Open_Level(LEVEL_LOGO)))
		return E_FAIL;

	ID3D11RasterizerState* RasterizerState;
	D3D11_RASTERIZER_DESC RSDesc;
	RSDesc.FillMode = D3D11_FILL_SOLID;
	RSDesc.CullMode = D3D11_CULL_BACK;
	RSDesc.FrontCounterClockwise = false;
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.ScissorEnable = false;
	RSDesc.MultisampleEnable = false;
	RSDesc.AntialiasedLineEnable = false;
	m_pDevice->CreateRasterizerState(&RSDesc, &RasterizerState);
	m_pContext->RSSetState(RasterizerState);
	
	return S_OK;
}

void CApplication::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Engine_Tick(TimeDelta);
	//m_pStageManager->Tick(TimeDelta);
}

HRESULT CApplication::Render()
{
	if (!m_pGameInstance || !m_pRenderer)
		return E_FAIL;

	m_pGUIManager->NewFrame();
	m_pGameInstance->RenderLevelUI();
	m_pGameInstance->RenderGUI();
	//m_pStageManager->RenderGUI();
	m_pGUIManager->Render();

	m_pGameInstance->Clear_RenderTargetView(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencilView();

	m_pRenderer->Draw();
	m_pGameInstance->CollisionRender();
	m_pGUIManager->RenderDrawData();

	m_pGameInstance->Present();

	return S_OK;
}

HRESULT CApplication::Open_Level(LEVEL_ID eLevelID)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT hr = 0;
	wstring message;
	switch (eLevelID)
	{
	case LEVEL_STATIC:
	case LEVEL_LOADING:
		message = L"CApplication::Open_Level : index check";
		MESSAGE(message);
		break;
	default:
		hr = m_pGameInstance->Open_Level(LEVEL_LOADING, CLevel_Loading::Create(m_pDevice, m_pContext, eLevelID));
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::Ready_Prototype_Static_Component()
{
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_renderer"),
		m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_vibuffer_rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXTEX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_groud"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_GROUD.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_phong"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_PHONG.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("proto_com_texture_background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/background.jpg")))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CApplication::Ready_Prototype_Static_GameObject()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_dynamic_camera"),
		CDynamicCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_background"),
		CBackGround::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_wall"), CWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

HRESULT CApplication::InitializeManager()
{
	CApplicationManager::GetInstance();
	CStageCollisionManager::GetInstance();

	return S_OK;
}

void CApplication::DestroyManager()
{
	m_pGUIManager->Shutdown();
	CApplicationManager::DestroyInstance();
	CStageCollisionManager::DestroyInstance();
}

CApplication* CApplication::Create()
{
	CApplication* pInstance = new CApplication();

	if (FAILED(pInstance->Initialize()))
	{
		wstring message = L"Failed to Create : CApplication";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CApplication::Free()
{
	DestroyManager();

	Safe_Release(m_pRenderer);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);

	Safe_Release(m_pGameInstance);
	CGameInstance::Engine_Release();
}
