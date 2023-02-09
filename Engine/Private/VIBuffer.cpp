#include "..\Public\VIBuffer.h"

CVIBuffer::CVIBuffer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CVIBuffer::CVIBuffer(const CVIBuffer& rhs)
	: CComponent(rhs)
	, m_pVB{ rhs.m_pVB }
	, m_pIB{ rhs.m_pIB }
	, m_iStride{ rhs.m_iStride }
	, m_iVertexCount{ rhs.m_iVertexCount }
	, m_iFaceIndexSize{ rhs.m_iFaceIndexSize }
	, m_iFaceCount{ rhs.m_iFaceCount }
	, m_iFaceIndexCount{ rhs.m_iFaceIndexCount }
	, m_iVertexBuffersCount{ rhs.m_iVertexBuffersCount }
	, m_eIndexFormat{ rhs.m_eIndexFormat }
	, m_eTopology{ rhs.m_eTopology }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CVIBuffer::Initialize(void * pArg)
{
	return S_OK;
}

HRESULT CVIBuffer::Render()
{
	if (nullptr == m_pContext)
		return E_FAIL;

	ID3D11Buffer* pVertexBuffers[] =
	{
		m_pVB,
	};

	_uint iStride[] =
	{
		m_iStride,
	};

	_uint iOffsets[] =
	{
		0,
	};

	/* 내가 사용하고자하는 버텍스 버퍼들을 장치에 바인딩한다. ( 장치는 다수의 버텍스 버퍼를 바인딩해놓을 수 있다.) */
	m_pContext->IASetVertexBuffers(0, m_iVertexBuffersCount, pVertexBuffers, iStride, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iFaceIndexCount * m_iFaceCount, 0, 0);

	return S_OK;
}

HRESULT CVIBuffer::Create_VertexBuffer()
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pVB);
}

HRESULT CVIBuffer::Create_IndexBuffer()
{
	return m_pDevice->CreateBuffer(&m_BufferDesc, &m_SubResourceData, &m_pIB);
}

void CVIBuffer::Free()
{
	__super::Free();

	Safe_Release(m_pVB);
	Safe_Release(m_pIB);
}
