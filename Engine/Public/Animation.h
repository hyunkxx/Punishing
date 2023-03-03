#pragma once

#include "Base.h"
#include "Channel.h"

// 모델에 정의되어 있는 어떤 한 동작의 시작과 끝정보를 갖음
// IDLE, ATTACK, HIT 등
BEGIN(Engine)

class CAnimation : public CBase
{
public:
	enum TYPE { ONE, LOOP };

	struct ANIMATION_DESC
	{
		_uint Clip;
		TYPE Type;
	};

private:
	CAnimation() = default;
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void PlayAnimation(_double TimeDelta, class CTransform* pTransform, TYPE eType, _bool bLerp, PREV_DATA PrevAnimation, const _double RatioValue, _bool bHoldY = false);

	const char* GetName() const { return m_szName; }
	void SetFinish(bool valus) { m_isFinish = valus; }
	_bool IsFinish() const { return m_isFinish; }
	_bool IsPreFinish() const { return m_isPreFinish; }
	void LocalReset() { m_LocalTime = 0.0; }
	void Reset();
	void LerpFinish();

	//Lerp
	const PREV_DATA& GetAnimationData() const { return m_PrevData; };
	_uint GetChannelCount() const { return m_iChannelsCount; }
	_bool IsLerpFinish() { return m_Channels[0]->IsLerpFinish(); }

private:
	void AnimationLerp(_double TimeDelta, CTransform* pTransform, PREV_DATA PrevData, const _double RatioValue, _bool bHoldAxisY = false);

	void PlayLoop(_double TimeDelta, CTransform* pTransform, _bool bHoldAxisY = false);
	void PlayOne(_double TimeDelta, CTransform* pTransform, _bool bHoldAxisY = false);

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free() override;

private:
	PREV_DATA m_PrevData;

	_uint m_iChannelsCount = { 0 };
	vector<class CChannel*> m_Channels;

	char m_szName[MAX_PATH] = "";

	//_uint m_iCurrentIndex;
	_double m_Duration = { 0.0 };
	_double m_TickPerSecond = { 0.0 };
	_double m_LocalTime = { 0.0 };

	_bool m_isFinish = false;
	_bool m_isPreFinish = false;
	_bool m_bLerpFinish = false;
};

END