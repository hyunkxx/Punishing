#include "..\Public\Model.h"

#include "Mesh.h"
#include "Bone.h"
#include "Channel.h"
#include "Shader.h"
#include "Texture.h"
#include "Animation.h"
#include "Transform.h"

CModel::CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CModel::CModel(const CModel& rhs)
	: CComponent(rhs)
	, m_pAIScene(rhs.m_pAIScene)
	, m_Materials(rhs.m_Materials)
	, m_iMaterialCount(rhs.m_iMaterialCount)
	, m_LocalMatrix(rhs.m_LocalMatrix)
	, m_eType(rhs.m_eType)
	, m_iAnimationCount(rhs.m_iAnimationCount)
{
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_AddRef(Material.pMaterialTexture[i]);
		}
	}

}

HRESULT CModel::Initialize_Prototype(MESH_TYPE eType, const char* pPath, _fmatrix LocalMatrix, _uint iAnimationCount)
{
	m_eType = eType;
	XMStoreFloat4x4(&m_LocalMatrix, LocalMatrix);
	
	m_iAnimationCount = iAnimationCount;

	_uint iFlags = aiProcessPreset_TargetRealtime_Fast | aiProcess_ConvertToLeftHanded;
	if (eType == MESH_TYPE::STATIC_MESH)
		iFlags |= aiProcess_PreTransformVertices;

	m_pAIScene = m_Importer.ReadFile(pPath, iFlags);
	if (nullptr == m_pAIScene)
		return E_FAIL;
	
	if (FAILED(InitializeMaterials(pPath)))

		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Initialize(void* pArg)
{
	if (FAILED(InitializeBones(m_pAIScene->mRootNode, nullptr)))
		return E_FAIL;
	m_iBoneCount = (_uint)m_Bones.size();

	if (FAILED(InitializeMesh(XMLoadFloat4x4(&m_LocalMatrix))))
		return E_FAIL;

	if (FAILED(InitializeAnimtaion(m_iAnimationCount)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::InitializeMesh(_fmatrix LocalMatrix)
{
	if (nullptr == m_pAIScene)
		return E_FAIL;

	m_iMeshCount = m_pAIScene->mNumMeshes;
	
	for(_uint i = 0 ; i < m_iMeshCount ; ++i)
	{
		CMesh* pMesh = CMesh::Create(m_pDevice, m_pContext, m_eType, m_pAIScene->mMeshes[i], this, LocalMatrix);
		if (nullptr == pMesh)
			return E_FAIL;

		pMesh->SetName(m_pAIScene->mMeshes[i]->mName.C_Str());
		m_Meshs.push_back(pMesh);

	}

	return S_OK;
}

HRESULT CModel::InitializeMaterials(const char * pModelFilePath)
{
	m_iMaterialCount = m_pAIScene->mNumMaterials;

	for(_uint i = 0 ; i < m_iMaterialCount ; ++i)
	{
		MATERIAL ModelMaterial;
		ZeroMemory(&ModelMaterial, sizeof(MATERIAL));

		aiMaterial* pAIMaterial = m_pAIScene->mMaterials[i];

		for(_uint j = 1 ; j < AI_TEXTURE_TYPE_MAX ; ++j)
		{
			aiString(strPath);

			if (FAILED(pAIMaterial->GetTexture(aiTextureType(j), 0, &strPath)))
			{
				ModelMaterial.pMaterialTexture[j] = nullptr;
				continue;
			}

			char szDrive[MAX_PATH] = "";
			char szPath[MAX_PATH] = "";
			_splitpath_s(pModelFilePath, szDrive, MAX_PATH, szPath, MAX_PATH, nullptr ,0 , nullptr, 0);

			char szFileName[MAX_PATH] = "";
			char szExt[MAX_PATH] = "";
			_splitpath_s(strPath.data, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, szExt, MAX_PATH);

			char szFullPath[MAX_PATH] = "";
			strcpy_s(szFullPath, szDrive);
			strcat_s(szFullPath, szPath);
			strcat_s(szFullPath, szFileName);
			strcat_s(szFullPath, szExt);


			_tchar szMaterialTexturePath[MAX_PATH] = TEXT("");
			MultiByteToWideChar(CP_ACP, 0, szFullPath, (int)strlen(szFullPath), szMaterialTexturePath, MAX_PATH);

			ModelMaterial.pMaterialTexture[j] = CTexture::Create(m_pDevice, m_pContext, szMaterialTexturePath);

			if (nullptr == ModelMaterial.pMaterialTexture[j])
				return E_FAIL;
		}

		m_Materials.push_back(ModelMaterial);
	}

	return S_OK;
}

HRESULT CModel::InitializeBones(aiNode* pAINode, CBone* pParent)
{
	CBone* pBone = CBone::Create(pAINode, pParent);
	if (nullptr == pBone)
		return E_FAIL;

	m_Bones.push_back(pBone);
	for (_uint i = 0; i < pAINode->mNumChildren ; ++i)
	{
		InitializeBones(pAINode->mChildren[i], pBone);
	}

	return S_OK;
}

HRESULT CModel::InitializeAnimtaion(_uint iAnimationCount)
{
	if (iAnimationCount > m_pAIScene->mNumAnimations)
		iAnimationCount = m_pAIScene->mNumAnimations;

 	m_iAnimationCount = iAnimationCount;
	for (_uint i = 0; i < m_iAnimationCount; ++i)
	{
 		CAnimation* pAnimation = CAnimation::Create(m_pAIScene->mAnimations[i], this);
		if (nullptr == pAnimation)
			return E_FAIL;

 		m_Animations.push_back(pAnimation);
	}

	return S_OK;
}

HRESULT CModel::Setup_ShaderMaterialResource(CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType)
{
	if (iMeshIndex >= m_iMeshCount)
		return E_FAIL;

	if (m_Materials[m_Meshs[iMeshIndex]->GetMaterialIndex()].pMaterialTexture[eType] == nullptr)
		return E_FAIL;

	return m_Materials[m_Meshs[iMeshIndex]->GetMaterialIndex()].pMaterialTexture[eType]->Setup_ShaderResource(pShader,pConstantName);
}

HRESULT CModel::Setup_BoneMatrices(CShader* pShader, const char* pConstantName, _uint iMeshIndex)
{
	if (iMeshIndex >= m_iMeshCount)
		return E_FAIL;

	_float4x4 MeshBoneMatrices[256];
	m_Meshs[iMeshIndex]->GetBoneMatrices(MeshBoneMatrices, XMLoadFloat4x4(&m_LocalMatrix));

	if (FAILED(pShader->SetMatrixArray(pConstantName, MeshBoneMatrices, 256)))
		return E_FAIL;

	return S_OK;
}

HRESULT CModel::Setup_Animation(_uint AnimationIndex , CAnimation::TYPE eType, _bool bLerp)
{
	if (AnimationIndex >= m_iAnimationCount)
		return E_FAIL;

	m_eAnimationType = eType;

	if (m_iCurrentAnimation != AnimationIndex)
	{
		m_bLerp = true;
		m_PrevData = m_Animations[m_iCurrentAnimation]->GetAnimationData();
		m_iPrevAnimation = m_iCurrentAnimation;
		m_iCurrentAnimation = AnimationIndex;
		m_Animations[m_iCurrentAnimation]->Reset();
		m_Animations[m_iPrevAnimation]->Reset();

		if (!bLerp)
			m_bLerp = false;
	}
	else if (m_Animations[m_iCurrentAnimation]->IsLerpFinish())
	{
		m_bLerp = false;
		m_Animations[m_iCurrentAnimation]->Reset();
		m_iCurrentAnimation = AnimationIndex;
	}
	return S_OK;
}

HRESULT CModel::Play_Animation(_double TimeDelta, CTransform * pTransform, const _double RatioValue, _bool bRootMotion)
{
	m_Animations[m_iCurrentAnimation]->PlayAnimation(TimeDelta, pTransform, m_eAnimationType, m_bLerp, m_PrevData, RatioValue, bRootMotion);

	for (auto& pBone : m_Bones)
		pBone->InvalidateCombinedMatrix();

	return S_OK;
}

HRESULT CModel::Render(_uint iMeshIndex)
{
	if (iMeshIndex >= m_iMeshCount)
		return E_FAIL;

	m_Meshs[iMeshIndex]->Render();

	return S_OK;
}

const char * CModel::GetMeshName(_uint iIndex)
{
	return  m_Meshs[iIndex]->GetName();
}

CBone* CModel::GetBonePtr(const char* pBoneName)
{
	//auto iter = find_if(m_Bones.begin(), m_Bones.end(), [&](CBone* pBone)
	//{
	//	return !strcmp(pBone->GetName(), pBoneName);
	//});
	CBone* Bone = nullptr;
	for (auto& pBone : m_Bones)
	{
		if (!strcmp(pBone->GetName(), pBoneName))
			Bone = pBone;
	}

	return Bone;
}

_bool CModel::AnimationIsFinish()
{
	_bool isFinish = false;
	if (isFinish = m_Animations[m_iCurrentAnimation]->IsFinish())
		m_Animations[m_iCurrentAnimation]->Reset();
	return isFinish;
}

//로컬타임은 리셋안함
_bool CModel::AnimationIsFinishEx()
{
	return m_Animations[m_iCurrentAnimation]->IsFinish();
}

//로컬타임은 리셋안함 & 한번떄리는데 0.5 듀레이션에서 종료
_bool CModel::AnimationIsPreFinish()
{
	return m_Animations[m_iCurrentAnimation]->IsPreFinish();
}

//로컬타임은 리셋안함 & 한번떄리는데 0.3 듀레이션에서 종료
_bool CModel::AnimationIsPreFinishEx()
{
	return m_Animations[m_iCurrentAnimation]->IsPreFinishEx();
}

_bool CModel::AnimationIsPreFinishCustom(_double value)
{
	return m_Animations[m_iCurrentAnimation]->IsPreFinishCustom(value);
}

void CModel::AnimationReset()
{
	m_Animations[m_iCurrentAnimation]->Reset();
}

void CModel::SetFinish(_bool Value)
{
	m_Animations[m_iCurrentAnimation]->SetFinish(Value);
}

CModel* CModel::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESH_TYPE eType, const char* pPath, _fmatrix LocalMatrix, _uint iAnimationCount)
{
	CModel* pInstance = new CModel(pDevice, pContext);
	if (FAILED(pInstance->Initialize_Prototype(eType, pPath, LocalMatrix, iAnimationCount)))
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
	
	for (auto& Material : m_Materials)
	{
		for (_uint i = 0; i < AI_TEXTURE_TYPE_MAX; ++i)
		{
			Safe_Release(Material.pMaterialTexture[i]);
		}
	}
	m_Materials.clear();

	for (auto& pMesh : m_Meshs)
		Safe_Release(pMesh);
	m_Meshs.clear();

	for (auto& pBone : m_Bones)
		Safe_Release(pBone);
	m_Bones.clear();

	for (auto& pAnimation : m_Animations)
		Safe_Release(pAnimation);
	m_Animations.clear();
}
