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
	_bool PlayAnimation(_double TimeDelta, class CTransform* pTransform, TYPE eType, _bool bLerp, vector<KEY_FRAME> CurrentKeyFrames);


	vector<KEY_FRAME> GetCurrentKeyFrames(_uint iIndex) { return m_Channels[iIndex]->GetCurrentKeyFrame(); }
	vector<class CChannel*>* GetChannel() { return &m_Channels; }
	const char* GetName() const { return m_szName; }
	CChannel* GetChannelIndex(_uint iIndex) const { return m_Channels[iIndex]; };

	void SetFinish(bool valus) { m_isFinish = valus; }
	_bool IsFinish() const { return m_isFinish; }
	void Reset();

private:
	_bool PlayLoop(_double TimeDelta, CTransform* pTransform, _bool bLerp);
	_bool PlayOne(_double TimeDelta, CTransform* pTransform, _bool bLerp);

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free() override;

private:
	_uint m_iChannelsCount = { 0 };
	vector<class CChannel*> m_Channels;

	char m_szName[MAX_PATH] = "";

	_uint m_iCurrentIndex;
	_double m_Duration = { 0.0 };
	_double m_TickPerSecond = { 0.0 };
	_double m_LocalTime = { 0.0 };

	_bool m_isFinish = false;
	_bool m_bLerpFinish = false;

	vector<KEY_FRAME> m_CurrentKeyFrames;

};

END