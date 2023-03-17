#pragma once

#include "Base.h"
#include "SkillBase.h"

BEGIN(Client)

class CSkillBallSystem final : public CBase
{
public:
	DECLARE_SINGLETON(CSkillBallSystem)

public:
	enum { CHAIN_ONE, CHAIN_TWO, CHAIN_THREE };

private:
	CSkillBallSystem() = default;
	~CSkillBallSystem() = default;

public:
	_uint TotalSkillCount() const { return m_iSkillCount; }
	_uint TotalSkillCountEx() const { return m_pSkills.size() + m_pReserveSkills.size(); }

	void Clear();

	void PushReadyTimer(_double DeltaTime);
	_bool IsPushReady() const { return m_bSkillPushReady; }

	HRESULT PushSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, CSkillBase::TYPE eType);
	CSkillBase::SKILL_INFO UseSkill(_uint iIndex);

	CSkillBase::CHAIN_DATA ComputeSkillChain(_uint iIndex);

public:
	virtual void Free() override;

private:
	_bool m_bSkillPushReady = true;
	_float m_fReadyTimer = 0.0f;
	const _float m_fReadyTimeOut = 0.5f;

	_uint m_iSkillCount = 0;
	vector<CSkillBase*> m_pSkills;
	queue<CSkillBase*> m_pReserveSkills;

};

END