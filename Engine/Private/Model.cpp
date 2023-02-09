#include "..\Public\Model.h"

#include "Mesh.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
{
}

HRESULT CModel::Initialize_Prototype(MESH_TYPE eType, const char* pPath)
{
	_uint iFlags = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;
	if (eType == MESH_TYPE::STATIC_MESH)
		iFlags |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pPath, iFlags);

	if (FAILED(InitalizeMesh()))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CModel::InitalizeMesh()
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iMeshCount = m_pAIScene->mNumMeshes;


	for(_uint i = 0 ; i < m_iMeshCount ; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_pAIScene->mMeshes[i]);
		if (nullptr == pMesh)
			return E_FAIL;

		m_vecMesh.push_back(pMesh);
	}

	return S_OK;
}

HRESULT CModel::Render()
{
	for(auto& pMesh : m_vecMesh)
	{
		if(nullptr != pMesh)
			pMesh->Render();
	}

	return S_OK;
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESH_TYPE eType, const char* pPath)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, pPath)))
	{
		MSG_BOX("Failed to Create : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CModel::Clone(void* pArg)
{
	CModel* pInstance = new CModel(*this);
	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Clone : CModel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CModel::Free()
{
	__super::Free();

	if(false == m_isClone)
	{
		for (auto& pMesh : m_vecMesh)
			Safe_Release(pMesh);
	}
}
