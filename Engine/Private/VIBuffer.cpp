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
	, m_iVerticesCount{ rhs.m_iVerticesCount }
	, m_iPrimitiveIndexSize{ rhs.m_iPrimitiveIndexSize }
	, m_iPrimitiveCount{ rhs.m_iPrimitiveCount }
	, m_iPrimitiveIndexCount{ rhs.m_iPrimitiveIndexCount }
	, m_iVertexBuffersCount{ rhs.m_iVertexBuffersCount }
	, m_eIndexFormat{ rhs.m_eIndexFormat }
	, m_eTopology{ rhs.m_eTopology }
{
	Safe_AddRef(m_pVB);
	Safe_AddRef(m_pIB);
}

HRESULT CVIBuffer::Initialize_Protorype()
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

	/* ���� ����ϰ����ϴ� ���ؽ� ���۵��� ��ġ�� ���ε��Ѵ�. ( ��ġ�� �ټ��� ���ؽ� ���۸� ���ε��س��� �� �ִ�.) */
	m_pContext->IASetVertexBuffers(0, m_iVertexBuffersCount, pVertexBuffers, iStride, iOffsets);
	m_pContext->IASetIndexBuffer(m_pIB, m_eIndexFormat, 0);
	m_pContext->IASetPrimitiveTopology(m_eTopology);

	m_pContext->DrawIndexed(m_iPrimitiveIndexCount * m_iPrimitiveCount, 0, 0);

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