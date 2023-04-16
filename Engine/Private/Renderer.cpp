#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "IAlphaSortable.h"
#include "GameInstance.h"

#include "PostEffect.h"
#include "BlurEffect.h"
#include "ScreenBlurEffect.h"
#include "DistortionEffect.h"
#include "Shadow.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Initialize_Prototype()
{
	m_pPostEffect = new CPostEffect(m_pDevice, m_pContext);
	m_pBlurEffect = new CBlurEffect(m_pDevice, m_pContext);
	m_pScreenBlurEffect = new CScreenBlurEffect(m_pDevice, m_pContext);
	m_pDistortionEffect = new CDistortionEffect(m_pDevice, m_pContext);
	m_pShadow = new CShadow(m_pDevice, m_pContext);

	m_pPostEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_POSTEFFECT.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	m_pBlurEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/POST_PROCESSING_RGBBLUR.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	m_pScreenBlurEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_BLUR.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	m_pDistortionEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_DISTORTION.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	m_pShadowShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_SHADOW.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);

	m_pPostEffect->SetBufferSize(0.f, 0.f, 1280, 720);
	m_pBlurEffect->SetBufferSize(0.f, 0.f, 1280, 720);
	m_pScreenBlurEffect->SetBufferSize(0.f, 0.f, 1280, 720);
	m_pDistortionEffect->SetBufferSize(0.f, 0.f, 1280, 720);
	m_pShadow->SetBufferSize(0.f, 0.f, 1280, 720);

	return S_OK;
}


HRESULT CRenderer::Add_RenderGroup(RENDER_GROUP eRenderGroup, CGameObject* pGameObject)
{
	if (nullptr == pGameObject || eRenderGroup >= RENDER_END)
		return E_FAIL;

	m_RenderObject[eRenderGroup].push_back(pGameObject);
	Safe_AddRef(pGameObject);

	return S_OK;
}

void CRenderer::Draw()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();

	//�̰�Ǯ�� ����Ʈ ���������
	//pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::BACK_BUFFER, _float4(0.f, 0.f, 1.f, 1.f));
	//pGameInstance->SetRenderTarget(POST_RENDERTARGET::BACK_BUFFER);
	//pGameInstance->Clear_DepthStencilView();

	//Pre �ؽ��Ŀ� ������ (Main & Bloom)
	//pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::CURRENT_BUFFER, _float4(0.f, 1.f, 0.f, 0.f));
	//pGameInstance->SetRenderTarget(POST_RENDERTARGET::CURRENT_BUFFER);

	// MAIN & BLOOM
	pGameInstance->Clear_PreRenderTargetViews(_float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->Clear_PreRenderTargetViews(PRE_RENDERTARGET::DISTORTION, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->SetPreRenderTargets();

	RenderPriority();
	RenderNonAlpha();
	RenderNonLight();
	RenderAlphaBlend();
	RenderEffect();

	//�׸��� ����
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::SHADOW_BUFFER, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::SHADOW_BUFFER);
	pGameInstance->Clear_DepthStencilView();
	ID3D11ShaderResourceView* pMain = pGameInstance->GetShaderResourceView(PRE_RENDERTARGET::MAIN);
	ID3D11ShaderResourceView* pDepth = pGameInstance->GetShaderResourceView(PRE_RENDERTARGET::DEPTH);
	ID3D11ShaderResourceView* pShadowDepth = pGameInstance->GetShaderResourceView(PRE_RENDERTARGET::SHADOW_DEPTH);
	m_pShadow->EffectApply(pMain, pDepth, pShadowDepth, m_pShadowShader);

	//���ΰ� �׸��ڸ� �ռ�
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::MAIN_SHADOW, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::MAIN_SHADOW);
	ID3D11ShaderResourceView* pScene = pGameInstance->GetShaderResourceView(PRE_RENDERTARGET::MAIN);
	ID3D11ShaderResourceView* pShadow = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::SHADOW_BUFFER);
	m_pPostEffect->BufferCombine(pScene, pShadow, m_pPostEffectShader, 2);

	//X��
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::XBLUR_BUFFER, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::XBLUR_BUFFER);
	pGameInstance->Clear_DepthStencilView();
	ID3D11ShaderResourceView* pXBlurSRV = pGameInstance->GetShaderResourceView(PRE_RENDERTARGET::BLOOM);
	m_pScreenBlurEffect->EffectApply(pXBlurSRV, m_pScreenBlurEffectShader, 0);

	//Y��
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::YBLUR_BUFFER, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::YBLUR_BUFFER);
	pGameInstance->Clear_DepthStencilView();
	ID3D11ShaderResourceView* pXYBlurSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::XBLUR_BUFFER);
	m_pScreenBlurEffect->EffectApply(pXYBlurSRV, m_pScreenBlurEffectShader, 1);

	//��� -> ��ó�� -> ���ο� �ռ�
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::CURRENT_BUFFER, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::CURRENT_BUFFER);
	pGameInstance->Clear_DepthStencilView();
	ID3D11ShaderResourceView* pBlurSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::YBLUR_BUFFER);
	ID3D11ShaderResourceView* pMainSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::MAIN_SHADOW);
	m_pPostEffect->EffectCombine(pMainSRV, pBlurSRV, m_pPostEffectShader);

	//����� : PRE ����� �ʱ� �÷� ���� ���� 0
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::POST_DISTORTION, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::POST_DISTORTION);
	pGameInstance->Clear_DepthStencilView();
	pMainSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::CURRENT_BUFFER);
	ID3D11ShaderResourceView* pDistortionSRV = pGameInstance->GetShaderResourceView(PRE_RENDERTARGET::DISTORTION);
	m_pDistortionEffect->TimeAcc();
	m_pDistortionEffect->EffectApply(pMainSRV, pDistortionSRV, m_pDistortionEffectShader);

	//Fin ���� ���ε� �� ������ �ؽ��Ŀ� �׷ô� ��ü ȭ���� Fin �ؽ��Ŀ� ������
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::FINAL_BUFFER, _float4(0.f, 0.f, 0.f, 0.f));
	pGameInstance->Clear_DepthStencilView();
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::FINAL_BUFFER);
	ID3D11ShaderResourceView* pFinSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::POST_DISTORTION);
	m_pBlurEffect->EffectApply(pFinSRV, m_pBlurEffectShader);//rgb�и� ����Ʈ

	//����� ���ε� - �ؽ��� ����
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::BACK_BUFFER, _float4(0.f, 0.f, 1.f, 0.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::BACK_BUFFER);

	ID3D11ShaderResourceView* pSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::FINAL_BUFFER);
	m_pPostEffect->EffectApply(pSRV, m_pPostEffectShader);

	RenderUI();
	RenderEnding();	//�̰� ī�޶󿡼� ���µ� �ϴ� ���� �ֽ���� ���ȳ�
}

void CRenderer::SetBlurAmount(_float fPower)
{
	m_pBlurEffect->SetBlurAmount(fPower);
}

bool Compute(Engine::CGameObject* pSourObject, Engine::CGameObject* pDestObject)
{
	_float fSourLength = pSourObject->GetLengthFromCamera();
	_float fDestLength = pDestObject->GetLengthFromCamera();

	return fSourLength > fDestLength;
}

void CRenderer::ZSort()
{
	//for (auto& pGameObject : m_RenderObject[RENDER_ALPHABLEND])
	//sort(m_RenderObject[RENDER_ALPHABLEND].begin(), m_RenderObject[RENDER_ALPHABLEND].end());
	m_RenderObject[RENDER_ALPHABLEND].sort(Compute);
}

CRenderer* CRenderer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CRenderer* pInstance = new CRenderer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		wstring message = L"Failed to Create : CRenderer";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CRenderer::Clone(void* pArg)
{
	AddRef();
	return this;
}

void CRenderer::Free()
{
	__super::Free();

	delete m_pPostEffect;
	delete m_pBlurEffect;

}

void CRenderer::RenderPriority()
{
	for (auto& pGameObject : m_RenderObject[RENDER_PRIORITY])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_PRIORITY].clear();
}

void CRenderer::RenderNonAlpha()
{
	for (auto& pGameObject : m_RenderObject[RENDER_NONALPHA])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_NONALPHA].clear();
}

void CRenderer::RenderNonLight()
{
	for (auto& pGameObject : m_RenderObject[RENDER_NONLIGHT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_NONLIGHT].clear();
}

void CRenderer::RenderAlphaBlend()
{
	ZSort();

	for (auto& pGameObject : m_RenderObject[RENDER_ALPHABLEND])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_ALPHABLEND].clear();
}

void CRenderer::RenderEffect()
{
	ZSort();

	for (auto& pGameObject : m_RenderObject[RENDER_EFFECT])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_EFFECT].clear();
}

void CRenderer::RenderUI()
{
	for (auto& pGameObject : m_RenderObject[RENDER_UI])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_UI].clear();
}

void CRenderer::RenderEnding()
{
	for (auto& pGameObject : m_RenderObject[RENDER_ENDING])
	{
		if (nullptr != pGameObject)
			pGameObject->Render();

		Safe_Release(pGameObject);
	}
	m_RenderObject[RENDER_ENDING].clear();
}
