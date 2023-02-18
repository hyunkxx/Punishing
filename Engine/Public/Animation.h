#pragma once

#include "Base.h"

// �𵨿� ���ǵǾ� �ִ� � �� ������ ���۰� �������� ����
// IDLE, ATTACK, HIT ��
BEGIN(Engine)

class CAnimation : public CBase
{
private:
	CAnimation() = default;
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(aiAnimation* pAIAnimation, class CModel* pModel);
	void PlayAnimation(_double TimeDelta);
	const char* GetName() const { return m_szName; }
	_uint GetChannelIndex(const char* pAnimationName) const;
	void Reset();

public:
	static CAnimation* Create(aiAnimation* pAIAnimation, class CModel* pModel);
	virtual void Free() override;

private:
	_uint m_iChannelsCount = { 0 };
	vector<class CChannel*> m_Channels;

	char m_szName[MAX_PATH] = "";
	_double m_Duration = { 0.0 };
	_double m_TickPerSecond = { 0.0 };
	_double m_LocalTime = { 0.0 };

	_bool m_isLoop = true;
	_bool m_isFinish = false;

};

END