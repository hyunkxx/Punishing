#include "pch.h"
#include "..\Public\GUIManager.h"

IMPLEMENT_SINGLETON(CGUIManager)

CGUIManager::CGUIManager()
{
}

void CGUIManager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
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

void CGUIManager::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CGUIManager::RenderDrawData()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void CGUIManager::Render()
{
	{
		ImGui::SetNextWindowPos(ImVec2(0, g_iWinSizeY - 30));
		ImGui::SetNextWindowSize(ImVec2(210, 20));
		ImGui::Begin("Frame", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
		ImGui::SameLine();
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}

	ImGui::ShowDemoWindow();

	ImGui::Begin("Hello, world!");
	ImGui::Button("TEST");
	ImGui::End();

	ImGui::Render();
}

void CGUIManager::Shutdown()
{
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	Safe_Release(m_pContext);
	Safe_Release(m_pDevice);

	CGUIManager::DestroyInstance();
}

void CGUIManager::Free()
{
}
