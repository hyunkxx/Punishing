#pragma once

#include "Base.h"
#include "Wall.h"

BEGIN(Client)

class CStageCollisionManager final : public CBase
{
	DECLARE_SINGLETON(CStageCollisionManager)

private:
	CStageCollisionManager() = default;
	virtual ~CStageCollisionManager() = default;

public:
	_bool IsActive() const { return m_isActive; }
	void SetActive(_bool value) { m_isActive = value; }
	void AddWall(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	void DeleteObject();
	void Tick(_double TimeDelta);
	void RenderGUI();

public:
	static HRESULT LoadCollisionData(const wchar_t * pPath, vector<CWall::CUBE_DESC>* CubeDescs_out, _uint* iCollisionCount_out);
	HRESULT SaveCollisionData();

public:
	virtual void Free() override;

private:
	_bool m_isActive = true;

	_uint m_iCurrentIndex = 0;
	vector<CWall*> m_Walls;

};

END