#pragma once

#include "Component.h"
#include "Animation.h"

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
	virtual HRESULT Initialize_Prototype(MESH_TYPE eType, const char* pPath, _fmatrix LocalMatrix, _uint iAnimationCount = 0);
	virtual HRESULT Initialize(void* pArg = nullptr);

public:
	HRESULT InitializeMesh(_fmatrix LocalMatrix);
	HRESULT InitializeMaterials(const char* pModelFilePath);
	HRESULT InitializeBones(aiNode* pAINode, class CBone* pParent);
	HRESULT InitializeAnimtaion(_uint iAnimationCount);

	HRESULT Setup_ShaderMaterialResource(class CShader* pShader, const char* pConstantName, _uint iMeshIndex, aiTextureType eType);
	HRESULT Setup_BoneMatrices(class CShader* pShader, const char* pConstantName, _uint iMeshIndex);
	HRESULT Setup_Animation(_uint AnimationIndex, CAnimation::TYPE eType, _bool bLerp);
	HRESULT Play_Animation(_double TimeDelta, class CTransform* pTransform, const _double RatioValue = 0.2, _bool bRootMotion = true, char* pRootBoneName = "Bip001");

	HRESULT Render(_uint iMeshIndex);

	const char* GetMeshName(_uint iIndex);
	_uint Get_MeshCount() { return m_iMeshCount; }
	class CBone* GetBonePtr(const char* pBoneName);
	_float4x4 GetLocalMatrix() const { return m_LocalMatrix; }

	// Animation
	_bool AnimationCompare(_uint AnimationIndex) const { return m_iCurrentAnimation == AnimationIndex; };
	_bool AnimationIsFinish();//리셋 O
	_bool AnimationIsFinishEx();//리셋 X
	_bool AnimationIsPreFinish(); //공격 타이밍 0.5
	_bool AnimationIsPreFinishEx(); //공격전 타이밍 0.3
	_bool AnimationIsPreFinishCustom(_double value); //종료 타이밍 지정
	_double GetCurrentTimeAcc() const;
	void AnimationReset();
	void SetFinish(_bool Value);

public:
	static CModel* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, MESH_TYPE eType, const char* pPath, _fmatrix LocalMatrix, _uint iAnimationCount = 0);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	CAnimation::TYPE m_eAnimationType = CAnimation::TYPE::ONE;

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
	_uint m_iPrevAnimation = { 0 };
	vector<class CAnimation*> m_Animations;

	//Lerp
	_bool m_bLerp = false;
	PREV_DATA m_PrevData;
	
};

END