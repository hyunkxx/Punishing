#include "pch.h"
#include "..\Public\ToolMain.h"
#include "Component_Manager.h"
#include "GameInstance.h"
#include "GUIManager.h"


#include "MapTool.h"

#include "ToolCamera.h"
#include "Terrain.h"
#include "Cube.h"

CToolMain::CToolMain()
	: m_pGUIManager{ CGUIManager::GetInstance() }
	, m_pGameInstance{ CGameInstance::GetInstance() }
{
	Safe_AddRef(m_pGameInstance);
}

HRESULT CToolMain::Initialize()
{
	GRAPHIC_DESC GraphicDesc;
	ZeroMemory(&GraphicDesc, sizeof GRAPHIC_DESC);
	GraphicDesc.eMode = GRAPHIC_DESC::MODE_WINDOW;
	GraphicDesc.iWinSizeX = g_iWinSizeX;
	GraphicDesc.iWinSizeY = g_iWinSizeY;
	GraphicDesc.hWnd = g_hWnd;
	GraphicDesc.hInstance = g_hInst;

	if (FAILED(m_pGameInstance->Engine_Initialize(GraphicDesc, TOOL_TYPE::TOOL_END, &m_pDevice, &m_pContext)))
		return E_FAIL;
	
	m_pGUIManager->Initialize(m_pDevice, m_pContext);

	if (FAILED(Initialize_Components()))
		return E_FAIL;
	
	if (FAILED(Initialize_GameObjects()))
		return E_FAIL;

	if (FAILED(Open_Level(TOOL_MAP)))
		return E_FAIL;

	return S_OK;
}

void CToolMain::Tick(_double TimeDelta)
{
	if (nullptr == m_pGameInstance)
		return;

	m_pGameInstance->Engine_Tick(TimeDelta);
}

void CToolMain::LateTick(_double TimeDelta)
{

}

void CToolMain::Render()
{
	m_pGUIManager->NewFrame();
	m_pGameInstance->RenderLevelUI();
	m_pGameInstance->RenderGUI();
	m_pGUIManager->Render();

	m_pGameInstance->Clear_RenderTargetView(_float4(0.f, 0.f, 1.f, 1.f));
	m_pGameInstance->Clear_DepthStencilView();

	m_pRenderer->Draw();
	m_pGUIManager->RenderDrawData();

	m_pGameInstance->Present();
}

HRESULT CToolMain::Open_Level(TOOL_TYPE eType)
{
	if (nullptr == m_pGameInstance)
		return E_FAIL;

	HRESULT hr = 0;
	wstring message;
	switch (eType)
	{
	case TOOL_STATIC:
		message = L"CApplication::Open_Level : index check";
		MESSAGE(message);
		break;
	case TOOL_MAP:
		hr = m_pGameInstance->Open_Level(TOOL_MAP, CMapTool::Create(m_pDevice, m_pContext));
		break;
	}

	if (FAILED(hr))
		return E_FAIL;

	return S_OK;
}

HRESULT CToolMain::Initialize_Components()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_renderer"), m_pRenderer = CRenderer::Create(m_pDevice, m_pContext))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_transform"),
		CTransform::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_vibuffer_rect"),
		CVIBuffer_Rect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_vibuffer_cube"),
		CVIBuffer_Cube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_vibuffer_triangle"),
		CVIBuffer_Triangle::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_vibuffer_terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Height.bmp")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_vibuffer_plane_terrain"),
		CVIBuffer_Terrain::Create(m_pDevice, m_pContext, {64,64}))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_shader_vtxtex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXTEX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_shader_alpha"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_ALPHA.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_shader_vtxnortex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXNORTEX.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_shader_vtxcube"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXCUBE.hlsl"), VTXCUBE_DECLARATION::Elements, VTXCUBE_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_shader_groud"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_GROUD.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_shader_phong"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_PHONG.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TOOL_STATIC, TEXT("proto_com_texture_terrain"), CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Terrain/Tile0.dds")))))
		return E_FAIL;

	Safe_AddRef(m_pRenderer);

	return S_OK;
}

HRESULT CToolMain::Initialize_GameObjects()
{
	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_terrain"), CTerrain::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_cube"), CCube::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_tool_camera"), CToolCamera::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	return S_OK;
}

CToolMain* CToolMain::Create()
{
	CToolMain* pInstance = new CToolMain();
	if (FAILED(pInstance->Initialize()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Create Failed : CToolMain");
	}
	return pInstance;
}

void CToolMain::Free()
{
	Safe_Release(m_pRenderer);
	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	Safe_Release(m_pGameInstance);
	CGameInstance::Engine_Release();
}
