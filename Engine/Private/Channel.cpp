#include "..\Public\Channel.h"
#include "Transform.h"
#include "Model.h"
#include "Bone.h"

HRESULT CChannel::Initialize(aiNodeAnim* pAIChannel, CModel* pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_pBone = pModel->GetBonePtr(m_szName);
	Safe_AddRef(m_pBone);

	m_iKeyFrameCount = { 0 };
	m_iCurrentIndex = { 0 };

	m_bLerpEnd = false;
	m_bAlignPivot = false;
	m_bAlignPosY = false;

	m_AlignLocal = 0.0;
	m_vPrevBonePos = { 0.f, 0.f, 0.f };
	m_vIdleOriginPos = { 0.00798827875f, 0.790838718f, 0.0612164661f };

	m_iKeyFrameCount = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iKeyFrameCount = max(m_iKeyFrameCount, pAIChannel->mNumPositionKeys);

	_float3 vScale;
	_float4 vRotation;
	_float3 vPosition;

	//Ű�������� �����̾ ������ ���¸� �����ϱ����� ���ʿ� ����
	for (_uint i = 0; i < m_iKeyFrameCount; ++i)
	{
		KEY_FRAME KeyFrame;
		ZeroMemory(&KeyFrame, sizeof(KeyFrame));

		if (pAIChannel->mNumScalingKeys > i)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}

		if (pAIChannel->mNumRotationKeys > i)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;

			KeyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}

		if (pAIChannel->mNumPositionKeys > i)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(_float3));
			KeyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		KeyFrame.vScale = vScale;
		KeyFrame.vRotation = vRotation;
		KeyFrame.vPosition = vPosition;

		if (i == 0)
		{
			m_CurrentKeyFrame = { KeyFrame.vScale ,KeyFrame.vRotation ,KeyFrame.vPosition };
		}

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

_bool CChannel::AlignPositionY(_double TimeDelta, CTransform* pTransform)
{
	_bool ret = false;

	if (pTransform == nullptr)
		return false;

	if (!strcmp(m_pBone->GetName(), "Bip001"))
	{
		if (!m_bAlignPosY)
		{
			m_AlignLocal = 0.0;
		}

		m_AlignLocal += TimeDelta / 0.5f;

		_vector vPosition;
		_vector vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentIndex].vPosition);
		_vector vDestPosition = { 0.00798827875f, 0.790838718f, 0.0612164661f, 1.f };

		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)m_AlignLocal);

		//ù��° �������� ��ġ�� �ִϸ��̼� ��Ʈ�� ����
		XMStoreFloat3(&m_vPrevBonePos, vPosition);
		vPosition = XMLoadFloat3(&m_vIdleOriginPos);

		_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);
		vPos = XMVectorSetY(vPos, XMVectorGetY(vPosition));
		pTransform->Set_State(CTransform::STATE_POSITION, vPos);
	}

	if (m_AlignLocal > 0.5)
		return true;

	return ret;
}

void CChannel::InvalidateTransform(_double TrackPosition, CTransform* pTransform, _bool bRootMotion)
{
	if (0.0 == TrackPosition)
		m_iCurrentIndex = 0;

	_vector vScale;
	_vector vRotation;
	_vector vPosition;

	KEY_FRAME LastKeyFrame = m_KeyFrames.back();
	if (TrackPosition >= LastKeyFrame.Time)
	{
		//�ִϸ��̼��� �����ٸ� ���� �������� ���¸� ����
		vScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&LastKeyFrame.vPosition);

		//���� �������϶� �Ǻ� ������ �ʱ�ȭ
		if (!strcmp(m_pBone->GetName(), "Bip001"))
		{
			vPosition = XMVectorSetY(vPosition, LastKeyFrame.vPosition.y);
			XMStoreFloat3(&m_vPrevBonePos, vPosition);
			vPosition = XMVectorSet(m_vIdleOriginPos.x, LastKeyFrame.vPosition.y, m_vIdleOriginPos. z, 1.f);
		}
	}
	else
	{
		//�ִϸ��̼��� ������� �ʾҰ� Ʈ���� ��ġ�� ������������ �ð��� �������� ��� ���������� ����
		while (TrackPosition >= m_KeyFrames[m_iCurrentIndex + 1].Time)
			++m_iCurrentIndex;

		//���� Ʈ���� ���൵�� (����������)0.f ~ 1.f(����������)�� ����
		_double Ratio = (TrackPosition - m_KeyFrames[m_iCurrentIndex].Time) /
			(m_KeyFrames[m_iCurrentIndex + 1].Time - m_KeyFrames[m_iCurrentIndex].Time);
		
		_vector vSourScale, vDestScale;
		_vector vSourRotation, vDestRotation;
		_vector vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentIndex + 1].vPosition);
		
		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);

		//���� Ű������ ����
		XMStoreFloat3(&m_CurrentKeyFrame.vScale, vScale);
		XMStoreFloat4(&m_CurrentKeyFrame.vRotation, vRotation);
		XMStoreFloat3(&m_CurrentKeyFrame.vPosition, vPosition);

		//�ش� ���� ���� ����� ��Ʈ�� 000����
		if (!strcmp(m_pBone->GetName(), "Bip001"))
		{
			_vector vMoveValue = vPosition - XMLoadFloat3(&m_vPrevBonePos);
			_vector vFixMoveValue; // 180�� ȸ�� (X,Y�� * -1) & �÷��̾��� ����� ������ ����

			_vector vRight = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_RIGHT));
			_vector vUp = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_UP));
			_vector vLook = XMVector3Normalize(pTransform->Get_State(CTransform::STATE_LOOK));

			vFixMoveValue = XMVectorSet(
				XMVectorGetX(vMoveValue) * -1.f,
				XMVectorGetY(vMoveValue),
				XMVectorGetZ(vMoveValue) * -1.f,
				1.f);

			_vector vAlignPos = ((vRight * XMVectorGetX(vFixMoveValue)) + (vUp * XMVectorGetY(vFixMoveValue)) + (vLook * XMVectorGetZ(vFixMoveValue)));
			_vector vCurrentPos = pTransform->Get_State(CTransform::STATE_POSITION);

			//Ʈ������ ��ġ ����
			vAlignPos = XMVectorSetY(vAlignPos, 0.0);
			_vector vFinalPos = vCurrentPos + vAlignPos;

			if(pTransform != nullptr)
				pTransform->Set_State(CTransform::STATE_POSITION, vFinalPos);

			//���� Ű�������� �� ��ġ ����
			XMStoreFloat3(&m_vPrevBonePos, vPosition);

			//ù��° �������� ��ġ�� �ִϸ��̼� ��Ʈ�� ����

			if(bRootMotion)
				vPosition = XMVectorSet(m_vIdleOriginPos.x, XMVectorGetY(vPosition), m_vIdleOriginPos.z, 1.0f);
			else
				vPosition = XMVectorSet(0.f, 0.f, 0.f, 1.0f);
		}
	}

	// Ű�������� ���¸� ������ SRT�� �����Ͽ� �� ��� ������Ʈ
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pBone->SetTransformationMatrix(TransformationMatrix);
}

void CChannel::InvalidateTransformLerp(_double Ratio, CTransform * pTransform, PREV_DATA PrevData)
{
	_vector vScale;
	_vector vRotation;
	_vector vPosition;

	KEY_FRAME PrevKeyFrame;
	ZeroMemory(&PrevKeyFrame, sizeof(KEY_FRAME));

	_bool hasValue = false;
	for (_uint i = 0; i < PrevData.ChannelCount; ++i)
	{
		if (Equal((*PrevData.pChannels)[i]->GetName()))
		{
			PrevKeyFrame = (*PrevData.pChannels)[i]->GetCurrentKeyFrame();
			hasValue = true;
			break;
		}
	}

	if (false == hasValue)
		return;

	if (Ratio > 1.0)
	{
		Ratio = 1.0;
		m_bLerpEnd = true;
	}

	_vector vSourScale, vDestScale;
	_vector vSourRotation, vDestRotation;
	_vector vSourPosition, vDestPosition;

	vSourScale = XMLoadFloat3(&PrevKeyFrame.vScale);
	vSourRotation = XMLoadFloat4(&PrevKeyFrame.vRotation);
	vSourPosition = XMLoadFloat3(&PrevKeyFrame.vPosition);

	vDestScale = XMLoadFloat3(&m_KeyFrames[0].vScale);
	vDestRotation = XMLoadFloat4(&m_KeyFrames[0].vRotation);
	vDestPosition = XMLoadFloat3(&m_KeyFrames[0].vPosition);

	vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
	vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);

	//�ش� ���� ���� ����� ��Ʈ�� 000����
	if (!strcmp(m_pBone->GetName(), "Bip001"))
	{
		//ù��° �������� ��ġ�� �ִϸ��̼� ��Ʈ�� ����
		vPosition = XMVectorSetY(vPosition, PrevKeyFrame.vPosition.y);
		XMStoreFloat3(&m_vPrevBonePos, vPosition);
		//vPosition = XMVectorSet(m_vIdleOriginPos.x, PrevKeyFrame.vPosition.y, m_vIdleOriginPos.z, 1.f);
		vPosition = XMVectorSet(m_vIdleOriginPos.x, m_KeyFrames[0].vPosition.y, m_vIdleOriginPos.z, 1.f);
	}

	// Ű�������� ���¸� ������ SRT�� �����Ͽ� �� ��� ������Ʈ
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pBone->SetTransformationMatrix(TransformationMatrix);
}


_bool CChannel::Equal(const char* pName)
{
	return !strcmp(m_szName, pName);
}

CChannel* CChannel::Create(aiNodeAnim* pAIChannel, CModel* pModel)
{
	CChannel* pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pBone);
	m_KeyFrames.clear();
}
