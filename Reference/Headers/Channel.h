#pragma once

#include "Base.h"

BEGIN(Engine)

// 애니메이션을 구동하는데 필요한 뼈
class CChannel final: public CBase
{
private:
	CChannel() = default;
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(aiNodeAnim* pAIChannel, class CModel* pModel);
	void InvalidateTransform(_double TrackPosition);
	void Reset() { m_iCurrentKeyFrame = 0; }
	_bool Equal(const char* pName);

private:
	char m_szName[MAX_PATH] = "";
	class CBone* m_pBone = { nullptr };
	
	_uint m_iKeyFrameCount = { 0 };
	_uint m_iCurrentKeyFrame = { 0 };
	vector<KEY_FRAME> m_KeyFrames;

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free() override;

};

END