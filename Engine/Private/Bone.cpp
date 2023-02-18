#include "..\Public\Bone.h"

HRESULT CBone::Initialize(aiNode* pAINode, CBone* pParent)
{
	m_pParent = pParent;
	Safe_AddRef(pParent);

	strcpy_s(m_szName, pAINode->mName.data);
	
	memcpy(&m_TransformationMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	XMStoreFloat4x4(&m_TransformationMatrix, XMMatrixTranspose(XMLoadFloat4x4(&m_TransformationMatrix)));
	
	XMStoreFloat4x4(&m_CombinedMatrix, XMMatrixIdentity());

	return S_OK;
}

void CBone::InvalidateCombinedMatrix()
{
	if (nullptr == m_pParent)
	{
		m_CombinedMatrix = m_TransformationMatrix;
		return;
	}

	XMStoreFloat4x4(&m_CombinedMatrix, XMLoadFloat4x4(&m_TransformationMatrix) * XMLoadFloat4x4(&m_pParent->m_CombinedMatrix));
}

CBone* CBone::Create(aiNode* pAINode, CBone* pParent)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent)))
	{
		MSG_BOX("Failed to Created : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}
