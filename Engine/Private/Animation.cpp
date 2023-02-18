#include "..\Public\Animation.h"
#include "Channel.h"

HRESULT CAnimation::Initialize(aiAnimation* pAIAnimation, CModel* pModel)
{
	strcpy_s(m_szName, pAIAnimation->mName.data);
	
	m_Duration = pAIAnimation->mDuration;
	m_TickPerSecond = pAIAnimation->mTicksPerSecond;
	m_iChannelsCount = pAIAnimation->mNumChannels;

	for (_uint i = 0; i < m_iChannelsCount; ++i)
	{
		CChannel* pChannel = CChannel::Create(pAIAnimation->mChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

void CAnimation::PlayAnimation(_double TimeDelta)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;

	if (m_LocalTime >= m_Duration)
	{
		if (true == m_isLoop)
		{
			m_LocalTime = 0.0;
			m_isFinish = true;
			Reset();
		}
		else
		{
			m_isFinish = true;
		}
	}

	if (false == m_isFinish || true == m_isLoop)
	{
		for (auto& pChannel : m_Channels)
		{
			pChannel->InvalidateTransform(m_LocalTime);
		}
	}
}

_uint CAnimation::GetChannelIndex(const char * pAnimationName) const
{
	return 0;
}

void CAnimation::Reset()
{
	for (auto& pChannel : m_Channels)
	{
		pChannel->Reset();
	}
}

CAnimation* CAnimation::Create(aiAnimation* pAIAnimation, CModel* pModel)
{
	CAnimation* pInstance = new CAnimation();
	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Created : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
