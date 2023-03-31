#include "pch.h"
#include "..\Public\Application.h"

#include "ApplicationManager.h"
#include "StageCollisionManager.h"
#include "SkillBallSystem.h"
#include "GameInstance.h"
#include "ImGUIManager.h"

#include "Level_Loading.h"
#include "SwordTrail.h"

#include "Boss.h"
#include "BackGround.h"
#include "DynamicCamera.h"
#include "Wall.h"

//UI
#include "SkillBase.h"
#include "PlayerIcon.h"
#include "PlayerHealthBar.h"
#include "EnemyHealthBar.h"
#include "DamageFont.h"

_uint CApplication::s_TickCount = 0;

CApplication::CApplication()
	: m_pGameInstance { CGameInstance::GetInstance() }
	, m_pGUIManager { CImGUIManager::GetInstance() }
	, m_pStageManager { CStageCollisionManager::GetInstance() }
	, m_pSkillSystem { CSkillBallSystem::GetInstance() }
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
	RSDesc.CullMode = D3D11_CULL_NONE;
	RSDesc.FrontCounterClockwise = false;
	RSDesc.DepthBias = 0;
	RSDesc.DepthBiasClamp = 0;
	RSDesc.SlopeScaledDepthBias = 0;
	RSDesc.ScissorEnable = false;
	RSDesc.MultisampleEnable = false;
	RSDesc.AntialiasedLineEnable = false;
	m_pDevice->CreateRasterizerState(&RSDesc, &RasterizerState);
	m_pContext->RSSetState(RasterizerState);
	
	m_pGameInstance->SetCollisionDebugRender(false);

	return S_OK;
}

void CApplication::Tick(_double TimeDelta)
{
	s_TickCount++;
	if (s_TickCount > 30)
		s_TickCount = 0;

	srand((unsigned int)time(nullptr) * s_TickCount);

	if (nullptr == m_pGameInstance)
		return;

	//if (CApplicationManager::GetInstance()->IsHitFreeze())
	//{
	//	m_bHitFreezeLocal += TimeDelta;
	//	if (m_bHitFreezeLocal >= m_bHitFreezeTimeOut)
	//	{
	//		m_bHitFreezeLocal = 0.f;
	//		CApplicationManager::GetInstance()->SetHitFreeze(false);
	//	}

	//	TimeDelta = TimeDelta * 0.5f;
	//}

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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_swordtrail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_SWORDTRAIL.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_alpha"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_ALPHA.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_ui"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_UI.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_black"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Black.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_sword_mask1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Effect/SwordMask1.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_sword_mask2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Effect/SwordMask2.png")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("proto_com_texture_background"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/background.jpg")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("proto_com_texture_background2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/background2.jpg")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_LOADING, TEXT("proto_com_texture_loading"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Loading.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_combo_gage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/HealthBar/ComboGage.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_healthbar_back"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/HealthBar/HealthBar_back.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_healthbar_blood"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/HealthBar/HealthBar_blood.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_healthbar_front"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/HealthBar/HealthBar_front.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_player_back"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/HealthBar/PlayerBack.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_dash"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/HealthBar/dash.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_red"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/Red.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_blue"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/Blue.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_yellow"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/Yellow.png")))))
		return E_FAIL;

	//Key q w e r a s d f
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_a"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/A.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_s"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/S.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_d"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/D.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_f"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/F.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_q"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/Q.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_w"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/W.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_e"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/E.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_r"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/R.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/0.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/1.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/2.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/3.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/4.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/5.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_6"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/6.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_7"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/7.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_8"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/8.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_9"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/9.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_x"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/x.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_combo"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/Combo.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_skill_white"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/SkillWhite.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_skill_use"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/SkillUse.png")))))
		return E_FAIL;


	//버튼 아이콘들
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_attack_icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Icon/AttackIcon.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_dash_icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Icon/DashIcon.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_back_icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Icon/BackIcon.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_evolution_icon"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Icon/EvolutionIcon.png")))))
		return E_FAIL;


	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_target"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Icon/Target.png")))))
		return E_FAIL;

	//변신 게이지
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_evolution_back"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/EvolutionGageBack.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_evolution_front"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/EvolutionGageFront.png")))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_evolution_front"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/EvolutionGageFront.png")))))
	//	return E_FAIL;

	//
	// 무기는 정방향
	_matrix	LocalMatrix = XMMatrixIdentity();
	LocalMatrix = XMMatrixRotationY(XMConvertToRadians(180.0f));

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_boss",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::SKELETAL_MESH, "../../Resource/Mesh/Enemy/Boss/Boss.fbx", LocalMatrix, 20))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_enemyhp"), CEnemyHealthBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_playerhp"), CPlayerHealthBar::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_playericon"), CPlayerIcon::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_damagefont"), CDamageFont::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	//소드 트레일 이펙트
	_matrix SwordTrailMatrix = XMMatrixIdentity();
	SwordTrailMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_sword_trail",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Character/Kamui/Weapon/Effect/SwordTrail.fbx", SwordTrailMatrix))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_boss"),
		CBoss::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_wall"), CWall::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_skillball"), CSkillBase::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_sword_trail"), CSwordTrail::Create(m_pDevice, m_pContext))))
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
	CSkillBallSystem::DestroyInstance();
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