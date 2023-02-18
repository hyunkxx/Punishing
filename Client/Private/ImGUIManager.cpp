#include "pch.h"
#include "..\Public\ImGUIManager.h"

IMPLEMENT_SINGLETON(CImGUIManager)

CImGUIManager::CImGUIManager()
{
}

CImGUIManager::~CImGUIManager()
{
}

void CImGUIManager::Initialize(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	m_pDevice = pDevice;
	m_pContext = pContext;

	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	ImGui::StyleColorsDark();

	ImGui_ImplWin32_Init(g_hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pContext);
}

void CImGUIManager::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImGUIManager::RenderDrawData()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CImGUIManager::Render()
{
	{
		ImGui::SetNextWindowPos(ImVec2(0, g_iWinSizeY - 30));
		ImGui::SetNextWindowSize(ImVec2(210, 20));
		ImGui::Begin("Frame", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::SameLine();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::Render();
}

void CImGUIManager::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CImGUIManager::DestroyInstance();
}

void CImGUIManager::Free()
{
}
