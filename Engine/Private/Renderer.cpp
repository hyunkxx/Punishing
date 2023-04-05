#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "IAlphaSortable.h"
#include "GameInstance.h"

#include "PostEffect.h"
#include "BlurEffect.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Initialize_Prototype()
{
	//m_pPostEffect = new CPostEffect(m_pDevice, m_pContext);
	//m_pBlurEffect = new CBlurEffect(m_pDevice, m_pContext);

	//m_pPostEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_POSTEFFECT.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);
	//m_pBlurEffectShader = CShader::Create(m_pDevice, m_pContext, TEXT("../../Shader/SHADER_DISTORTION.hlsl"), VTXTEX_DECLARATION::Elements, VTXTEX_DECLARATION::ElementCount);

	//m_pPostEffect->SetBufferSize(0.f, 0.f, 1280, 720);
	//m_pBlurEffect->SetBufferSize(0.f, 0.f, 1280, 720);

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

	//이거풀면 디폴트 렌더링방식
	pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::BACK_BUFFER, _float4(0.f, 0.f, 1.f, 1.f));
	pGameInstance->SetRenderTarget(POST_RENDERTARGET::BACK_BUFFER);
	pGameInstance->Clear_DepthStencilView();

	////Pre 텍스쳐에 렌더링 (Main & Bloom)
	//pGameInstance->Clear_PreRenderTargetViews(_float4(0.f, 0.f, 1.f, 0.f));
	//pGameInstance->SetRenderTarget(POST_RENDERTARGET::CURRENT_BUFFER);

	RenderPriority();
	RenderNonAlpha();
	RenderNonLight();
	RenderAlphaBlend();

	//GG yo
	////Fin 버퍼 바인딩 후 기존에 텍스쳐에 그련던 전체 화면을 Fin 텍스쳐에 렌더링
	//pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::FINAL_BUFFER, _float4(0.f, 0.f, 1.f, 0.f));
	//pGameInstance->SetRenderTarget(POST_RENDERTARGET::FINAL_BUFFER);
	//ID3D11ShaderResourceView* pFinSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::CURRENT_BUFFER);
	//_float time = pGameInstance->GetDeltaTime();
	//m_pBlurEffect->TimeAcc(time);
	//m_pBlurEffect->EffectApply(pFinSRV, m_pBlurEffectShader);

	////백버퍼 바인딩
	//pGameInstance->Clear_RenderTargetView(POST_RENDERTARGET::BACK_BUFFER, _float4(0.f, 0.f, 1.f, 0.f));
	//pGameInstance->Clear_DepthStencilView();
	//pGameInstance->SetRenderTarget(POST_RENDERTARGET::BACK_BUFFER);

	////백버퍼에 텍스쳐 렌더링
	//ID3D11ShaderResourceView* pSRV;
	//pSRV = pGameInstance->GetShaderResourceView(POST_RENDERTARGET::FINAL_BUFFER);
	//m_pPostEffect->EffectApply(pSRV, m_pPostEffectShader);
	
	RenderUI();
	RenderEnding();	//이거 카메라에서 쓰는데 일단 냅둠 왜썼는지 기억안남
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

	//delete m_pPostEffect;
	//delete m_pBlurEffect;

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
