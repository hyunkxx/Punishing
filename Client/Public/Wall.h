#pragma once

#include "GameObject.h"

BEGIN(Engine)
class COBBCollider;
class CTransform;
END

BEGIN(Client)

class CWall final : public CGameObject
{
public:
	typedef struct tagCubeDesc
	{
		_float3 vScale;
		_float3 vRotation;
		_float3 vPosition;
	}CUBE_DESC;

protected:
	explicit CWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CWall(const CGameObject& rhs);
	virtual ~CWall() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void LateTick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void RenderGUI();

	_float GetDepth();
	_vector GetPosition();

	//파라미터로 넘긴 위치에서 제일 가까운 면의 노말벡터를 리턴
	_vector GetNormal(_fvector vPosition, _float* vLength_out);
	CUBE_DESC& GetDesc() { return m_CubeDesc; }

	void SetScale(_float3 vScale);
	void SetRotation(_float3 vRotation);
	void SetPosition(_float3 vPosition);

private:
	HRESULT AddComponents();
	HRESULT AddComponents_GamePlay();

public:
	static CWall* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;

private:
	COBBCollider* m_pCollider = nullptr;
	CTransform* m_pTransform = nullptr;

protected:
	enum { LEFT, RIGHT, FRONT, BACK };
	CUBE_DESC m_CubeDesc;
	_float3 m_vPlanePos[4]; //4개의 면의 중심위치 높이는 0

	//0이면 게임플레이 1이면 보스룸
	_int m_iLevelIndex = 0;

};

END