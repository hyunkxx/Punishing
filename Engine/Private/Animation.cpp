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

void CAnimation::PlayAnimation(_double TimeDelta, CTransform* pTransform, TYPE eType, _bool bLerp, PREV_DATA PrevData, const _double RatioValue, _bool bRootMotion)
{
	if (bLerp)
	{
		AnimationLerp(TimeDelta, pTransform, PrevData, RatioValue);
	}
	else
	{
		switch (eType)
		{
		case ONE:
			PlayOne(TimeDelta, pTransform, bRootMotion);
			break;
		case LOOP:
			PlayLoop(TimeDelta, pTransform, bRootMotion);
			break;
		}
	}
}

_bool CAnimation::IsPreFinishCustom(_double value)
{
	m_CustomFinishTime = value;
	return m_isCustomPreFinish;
}

void CAnimation::Reset()
{
	m_isFinish = false;
	m_isPreFinish = false;
	m_isPreFinishEx = false;
	m_isCustomPreFinish = false;
	m_LocalTime = 0.0;
	m_CustomFinishTime = 1.0;

	for (auto& pChannel : m_Channels)
	{
		pChannel->Reset();
	}
}

void CAnimation::LerpFinish()
{
	for (auto& pChannel : m_Channels)
	{
		
	}
}

void CAnimation::AnimationLerp(_double TimeDelta, CTransform * pTransform, PREV_DATA PrevData, const _double RatioValue, _bool bHoldAxisY)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;
	_double Ratio = m_LocalTime / RatioValue;

	for (auto& pChannel : m_Channels)
	{
		pChannel->InvalidateTransformLerp(Ratio, pTransform, PrevData);
	}

}

void CAnimation::PlayLoop(_double TimeDelta, CTransform * pTransform, _bool bHoldAxisY)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;
	if (m_LocalTime >= m_Duration)
	{
		m_isFinish = false;
		m_LocalTime = 0.0;
	}

	for (auto& pChannel : m_Channels)
	{
		pChannel->InvalidateTransform(m_LocalTime, pTransform, bHoldAxisY);
	}

}

void CAnimation::PlayOne(_double TimeDelta, CTransform * pTransform, _bool bHoldAxisY)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;//³·Ãç¾ß ´À·ÁÁü
	
	if (m_LocalTime >= m_Duration * m_CustomFinishTime)
		m_isCustomPreFinish = true;

	if (m_LocalTime >= m_Duration * 0.3)
		m_isPreFinishEx = true;

	if (m_LocalTime >= m_Duration * 0.5)
		m_isPreFinish = true;
	
	if (m_LocalTime >= m_Duration)
	{
		m_isFinish = true;
	}
	
	for (auto& pChannel : m_Channels)
	{
		pChannel->InvalidateTransform(m_LocalTime, pTransform, bHoldAxisY);
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
