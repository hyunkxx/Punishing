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
	void InvalidateTransform(_double TrackPosition, class CTransform* pTransform);
	void InvalidateTransformLerp(_double TrackPosition, _double Duration, class CTransform* pTransform, PREV_DATA PrevData);

	const char* GetName() const { return m_szName; }
	_bool Equal(const char* pName);
	void SetOriginPos(_float3 vPos) { m_vIdleOriginPos = vPos; }
	void Reset() { m_iCurrentIndex = 0;  m_bSetPivot = false; m_bLerpEnd = false; }

	//Lerp
	KEY_FRAME GetCurrentKeyFrame() const { return m_CurrentKeyFrame; }
	KEY_FRAME GetFirstKeyFrame() const { return m_KeyFrames[0]; }
	_bool IsLerpFinish() const { return m_bLerpEnd; }

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free() override;

private:
	char m_szName[MAX_PATH] = "";
	class CBone* m_pBone = { nullptr };

	_uint m_iKeyFrameCount = { 0 };
	_uint m_iCurrentIndex = { 0 };
	vector<KEY_FRAME> m_KeyFrames;
	KEY_FRAME m_CurrentKeyFrame;

	_bool m_bSetPivot = false;
	_float3 m_vPrevBonePos = { 0.f, 0.f, 0.f };
	_float3 m_vIdleOriginPos = { 0.00798827875f, 0.790838718f, 0.0612164661f };

	_bool m_bLerpEnd = false;
}; 

 

END