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
	
	return S_OK;
}

_bool CAnimation::PlayAnimation(_double TimeDelta, CTransform* pTransform, TYPE eType, _bool bLerp, vector<KEY_FRAME> CurrentKeyFrames)
{
	_bool bRet = false;
	m_CurrentKeyFrames = CurrentKeyFrames;

	switch (eType)
	{
	case ONE:
		bRet = PlayOne(TimeDelta, pTransform, bLerp);
		break;
	case LOOP:
		bRet = PlayLoop(TimeDelta, pTransform, bLerp);
		break;
	}

	return bRet;
}

void CAnimation::Reset()
{
	m_LocalTime = 0.0;
	m_isFinish = false;
	m_bLerpFinish = false;

	for (auto& pChannel : m_Channels)
	{
		pChannel->Reset();
	}
}

_bool CAnimation::PlayLoop(_double TimeDelta, CTransform* pTransform, _bool bLerp)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;

	if (m_LocalTime >= m_Duration)
	{
		m_LocalTime = 0.f;
	}
	
	if (false == bLerp)
	{
		for (auto& pChannel : m_Channels)
		{
			pChannel->InvalidateTransform(m_LocalTime, pTransform);
		}
	}
	else
	{
		for (auto& pChannel : m_Channels)
		{
			pChannel->InvalidateTransformLerp(m_LocalTime, m_Duration, pTransform, m_CurrentKeyFrames);
		}
	}

	return false;
}

_bool CAnimation::PlayOne(_double TimeDelta, CTransform* pTransform, _bool bLerp)
{
	m_LocalTime += m_TickPerSecond * TimeDelta;

	if (m_LocalTime >= m_Duration)
	{
		m_isFinish = true;
	}
	else
	{
		if (false == bLerp)
		{
			for (auto& pChannel : m_Channels)
			{
				pChannel->InvalidateTransform(m_LocalTime, pTransform);
			}
		}
		else
		{
			for (auto& pChannel : m_Channels)
			{
				pChannel->InvalidateTransformLerp(m_LocalTime, m_Duration, pTransform, m_CurrentKeyFrames);
			}
		}
	}


	return false;
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
