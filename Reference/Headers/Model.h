#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CModel final : public CComponent
{
public:
	enum MESH_TYPE{ STATIC_MESH, SKELETAL_MESH, TYPE_END };
private:
	CModel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CModel(const CModel& rhs);
	virtual ~CModel() = default;

public:
	virtual HRESULT Initialize_Prototype(MESH_TYPE eType, const char* pPath, _fmatrix LocalMatrix);
	virtual HRESULT Initialize(void* pArg = nullptr);

public:
	HRESULT InitializeMesh(_fmatrix LocalMatrix);
	HRESULT InitializeMaterials(const char* pModelFilePath);
	HRESULT InitializeBones(aiNode* pAINode, class CBone* pParent);
	HRESULT InitializeAnimtaion();

	HRESULT Setup_ShaderMaterialResource(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT Setup_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	HRESULT Setup_Animation(_uint AnimationIndex);
	HRESULT Play_Animation(_double TimeDelta);
	HRESULT Render(_uint iMeshIndex);

	_uint Get_MeshCount() { return m_iMeshCount; }
	class CBone* GetBonePtr(const char* pBoneName);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESH_TYPE eType, const char* pPath, _fmatrix LocalMatrix);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	_float4x4 m_LocalMatrix;
	MESH_TYPE m_eType = { TYPE_END };

	Assimp::Importer m_Importer;
	const aiScene* m_pAIScene = { nullptr };
	
	// Mesh
	_uint m_iMeshCount = { 0 };
	vector<class CMesh*> m_Meshs;

	// Material
	_uint m_iMaterialCount = { 0 };
	vector<MATERIAL> m_Materials;

	// Bone
	_uint m_iBoneCount = { 0 };
	vector<class CBone*> m_Bones;

	// Animation
	_uint m_iAnimationCount = { 0 };
	_uint m_iCurrentAnimation = { 0 };
	vector<class CAnimation*> m_Animations;
};

END