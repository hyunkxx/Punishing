#include "..\Public\Animation.h"
#include "Transform.h"
#include "Channel.h"
#include "Bone.h"

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

	m_PrevData.szName = m_szName;
	m_PrevData.ChannelCount = m_iChannelsCount;
	m_PrevData.pChannels = &m_Channels;

	return S_OK;
}

void CAnimation::PlayAnimation(_double TimeDelta, CTransform* pTransform, TYPE eType, _bool bLerp, PREV_DATA PrevData)
{
	if (bLerp)
	{
		switch (eType)
		{
		case ONE:
			PlayOneLerp(TimeDelta, pTransform, PrevData);
			break;
		case LOOP:
			PlayLoopLerp(TimeDelta, pTransform, PrevData);
			break;
		}
	}
	else
	{
		switch (eType)
		{
		case ONE:
			PlayOne(TimeDelta, pTransform);
			break;
		case LOOP:
			PlayLoop(TimeDelta, pTransform);
			break;
		}
	}
}

void CAnimation::Reset()
{
	m_isFinish = false;
	m_LocalTime = 0.0;

	for (auto& pChannel : m_Channels)
	{
		pChannel->Reset();
	}
}

void CAnimation::PlayLoopLerp(_double TimeDelta, CTransform * pTransform, PREV_DATA PrevData)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;

	for (auto& pChannel : m_Channels)
	{
		pChannel->InvalidateTransformLerp(m_LocalTime, m_Duration, pTransform, PrevData);
	}
}

void CAnimation::PlayOneLerp(_double TimeDelta, CTransform * pTransform, PREV_DATA PrevData)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;

	for (auto& pChannel : m_Channels)
	{
		pChannel->InvalidateTransformLerp(m_LocalTime, m_Duration, pTransform, PrevData);
	}
}

void CAnimation::PlayLoop(_double TimeDelta, CTransform * pTransform)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;

	if (m_LocalTime >= m_Duration)
	{
		m_isFinish = false;
		m_LocalTime = 0.0;
	}

	for (auto& pChannel : m_Channels)
	{
		pChannel->InvalidateTransform(m_LocalTime, pTransform);
	}
}

void CAnimation::PlayOne(_double TimeDelta, CTransform * pTransform)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;

	if (m_LocalTime >= m_Duration)
	{
		m_isFinish = true;
	}

	for (auto& pChannel : m_Channels)
	{
		pChannel->InvalidateTransform(m_LocalTime, pTransform);
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
