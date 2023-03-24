#include "..\Public\Renderer.h"
#include "GameObject.h"
#include "IAlphaSortable.h"

CRenderer::CRenderer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

HRESULT CRenderer::Initialize_Prototype()
{
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
	//for (auto& RenderList : m_RenderObject)
	for (int i = 0; i < RENDER_END; ++i)
	{
		if (i == RENDER_ALPHABLEND)
			ZSort();

		for (auto& pGameObject : m_RenderObject[i])
		{
			if (nullptr != pGameObject)
				pGameObject->Render();

			Safe_Release(pGameObject);
		}
		m_RenderObject[i].clear();
	}
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
}