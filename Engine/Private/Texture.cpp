#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CTexture::CTexture(const CTexture& rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
{
	for (auto& pSRV : m_Textures)
		Safe_AddRef(pSRV);
}

HRESULT CTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iTextureCount)
{
	m_Textures.reserve(iTextureCount);

	_tchar szFullPath[MAX_PATH] = TEXT("");
	for (_uint i = 0; i < iTextureCount; ++i)
	{
		ID3D11ShaderResourceView* pSRV = nullptr;
		wsprintf(szFullPath, pTextureFilePath, i);

		_tchar szExt[MAX_PATH] = TEXT("");
		//D:\Burger\132\Framework\Client\Bin\Resources\Textures\Snow\Snow.png
		_wsplitpath_s(szFullPath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT hr = 0;

		if (!lstrcmp(szExt, TEXT(".tga")))
		{
			MSG_BOX(".tga 파일을 로드할 수 없습니다.");
			return E_FAIL;
		}

		/* .dds	*/
		if (!lstrcmp(szExt, TEXT(".dds")))
		{
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}
		/* .jpg, .bmp, .png */
		else
		{
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szFullPath, nullptr, &pSRV);
		}

		if (FAILED(hr))
			return E_FAIL;

		m_Textures.push_back(pSRV);
	}

	return S_OK;
}

HRESULT CTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CTexture::Setup_ShaderResource(CShader* pShaderComponent, const char* pConstantName, _uint iTextureIndex)
{
 	if (nullptr == pShaderComponent || iTextureIndex >= m_Textures.size())
		return E_FAIL;

	return pShaderComponent->SetShaderResourceView(pConstantName, m_Textures[iTextureIndex]);
}

HRESULT CTexture::Setup_ShaderResourceArray(CShader * pShaderComponent, const char * pConstantName)
{
	if (nullptr == pShaderComponent)
		return E_FAIL;

	ID3D11ShaderResourceView* pSRVs[8] = { nullptr };

	_uint iTextureIndex = 0;
	for (auto& pSRV : m_Textures)
		pSRVs[iTextureIndex++] = pSRV;

	return pShaderComponent->SetShaderResourceViewArray(pConstantName, pSRVs, (int)m_Textures.size());
}

CTexture* CTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iTextureCount)
{
	CTexture* pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iTextureCount)))
	{
		wstring message = L"Failed to Create : CTexture";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CTexture::Clone(void* pArg)
{
	CTexture* pInstance = new CTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		wstring message = L"Failed to Clone : CTexture";
		MESSAGE(message);
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_Textures)
		Safe_Release(pSRV);

	m_Textures.clear();
}