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
	_bool InvalidateTransformLerp(_double TrackPosition, _double Duration, class CTransform* pTransform, vector<KEY_FRAME> CurrentKeyFrame);

	void Reset() { m_iCurrentKeyFrame = 0;  m_bSetPivot = false; m_bLerpFinish = false; }
	_bool IsLerpFinish() const { return m_bLerpFinish; }
	_bool Equal(const char* pName);
	void SetOriginPos(_float3 vPos) { m_vIdleOriginPos = vPos; }
	vector<KEY_FRAME> GetCurrentKeyFrame() const { return m_KeyFrames; }
	KEY_FRAME GetFirstKeyFrame() const { return m_KeyFrames[0]; }

private:
	char m_szName[MAX_PATH] = "";
	class CBone* m_pBone = { nullptr };

	_uint m_iKeyFrameCount = { 0 };
	_uint m_iCurrentKeyFrame = { 0 };
	vector<KEY_FRAME> m_KeyFrames;
	vector<KEY_FRAME> m_CurrentKeyFrame;

	_bool m_bSetPivot = false;
	_bool m_bLerpFinish = false;
	_float3 m_vPrevBonePos = { 0.f, 0.f, 0.f };
	_float3 m_vIdleOriginPos = { 0.00798827875f, 0790838718.f, 0.0612164661f };

public:
	static CChannel* Create(aiNodeAnim* pAIChannel, class CModel* pModel);
	virtual void Free() override;

};

END