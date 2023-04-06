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

#include "SpawnEffect.h"

//instance
#include "Flower.h"

//
#include "FloorCircle.h"

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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_vibuffer_instance_rect"),
		CVIBuffer_Rect_Instance::Create(m_pDevice, m_pContext, 50.f, 40.f, 50.f, 5.f, 30.f, 30))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_vtxtex"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXTEX.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_trail"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_THORNTRAIL.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_groud"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_GROUD.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_phong"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_PHONG.hlsl"), VTXNORTEX_DECLARATION::Elements, VTXNORTEX_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_freeze"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_FREEZEAREA.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_vtxmodel"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXMODEL.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::ElementCount))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_spawneffect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_SPAWNEFFECT.hlsl"), VTXMODEL_DECLARATION::Elements, VTXMODEL_DECLARATION::ElementCount))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_shader_vtxinstance_rect"),
		CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_VTXINSTANCE_RECT.hlsl"), VTXINSTANCE_DECLARATION::Elements, VTXINSTANCE_DECLARATION::ElementCount))))
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
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/0.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/1.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/2.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/3.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_4"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/4.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_5"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/5.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_6"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/6.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_7"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/7.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_8"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/8.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_9"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/9.dds")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_x"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Text/x.dds")))))
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

	//타겟 이미지
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_targetimage"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/dds/Target%d.dds"), 138))))
		return E_FAIL;

	//프리즈 에어리어
	//무지개색 노이즈
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_freezemask0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Noise.dds")))))
		return E_FAIL;

	//테두리 연하게
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_freezemask1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Mask100.dds")))))
		return E_FAIL;

	//마스크 이미지 == 디졸브 이미지
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_freezemask2"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Mask114.jpg")))))
		return E_FAIL;

	//이거 초산 마스크이미지
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_freezemask3"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/DarknessMask.jpg")))))
		return E_FAIL;

	//스폰 디퓨즈 이미지
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_spawneffect0"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/SpawnDiffuse.png")))))
		return E_FAIL;
	//스폰 마스크 이미지
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_spawneffect1"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/SpawnMask.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_smokediffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Smoke.png")))))
		return E_FAIL;
	
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_smokemask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Mask408.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_light"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Light.png")))))
		return E_FAIL;

	//가시
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_thorndiffuse"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Thorn/diffuse.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_thornmask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Thorn/mask.png")))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_gardmask"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Daoguang006.png")))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_trail"),
		CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/MaskImage/Trail.png")))))
		return E_FAIL;

	//if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, TEXT("proto_com_texture_evolution_front"),
	//	CTexture::Create(m_pDevice, m_pContext, TEXT("../../Resource/Texture/Image/Skill/EvolutionGageFront.png")))))
	//	return E_FAIL;

	//
	// 무기는 정방향

	_matrix	LocalMatrix = XMMatrixIdentity();

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_spawneffect",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Enemy/SpawnEffect.fbx", LocalMatrix))))
		return E_FAIL;

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

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_thorngard",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Enemy/Boss/ThornGard.fbx", SwordTrailMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_circle1",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Character/Kamui/PlayerCircle.fbx", SwordTrailMatrix))))
		return E_FAIL;
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_circle2",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Character/Kamui/EnemyCircle.fbx", SwordTrailMatrix))))
		return E_FAIL;

	SwordTrailMatrix = XMMatrixRotationY(XMConvertToRadians(180.f));
	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_sword_trail",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Character/Kamui/Weapon/Effect/SwordTrail.fbx", SwordTrailMatrix))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(LEVEL_STATIC, L"proto_com_model_sword_trail2",
		CModel::Create(m_pDevice, m_pContext, CModel::MESH_TYPE::STATIC_MESH, "../../Resource/Mesh/Character/Kamui/Weapon/Effect/SwordEffect2.fbx", SwordTrailMatrix))))
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

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_spawneffect"), CSpawnEffect::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_flower"), CFlower::Create(m_pDevice, m_pContext))))
		return E_FAIL;

	if (FAILED(m_pGameInstance->Add_Prototype(TEXT("proto_obj_circle"), CFloorCircle::Create(m_pDevice, m_pContext))))
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