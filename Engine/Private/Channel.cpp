#include "..\Public\Channel.h"
#include "Transform.h"
#include "Model.h"
#include "Bone.h"

HRESULT CChannel::Initialize(aiNodeAnim* pAIChannel, CModel* pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_pBone = pModel->GetBonePtr(m_szName);
	Safe_AddRef(m_pBone);

	m_iKeyFrameCount = max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys);
	m_iKeyFrameCount = max(m_iKeyFrameCount, pAIChannel->mNumPositionKeys);

	ZeroMemory(&m_CurrentKeyFrame, sizeof(KEY_FRAME));

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

		m_KeyFrames.push_back(KeyFrame);
	}

	return S_OK;
}

void CChannel::InvalidateTransform(_double TrackPosition, CTransform* pTransform)
{
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
			m_vPrevBonePos;
			XMStoreFloat3(&m_vPrevBonePos, vPosition);
			vPosition = XMLoadFloat3(&m_vIdleOriginPos);
		}

		Reset();
	}
	else
	{
		//�ִϸ��̼��� ������� �ʾҰ� Ʈ���� ��ġ�� ������������ �ð��� �������� ��� ���������� ����
		while (TrackPosition >= m_KeyFrames[m_iCurrentKeyFrame + 1].Time)
			++m_iCurrentKeyFrame;

		//���� Ʈ���� ���൵�� (����������)0.f ~ 1.f(����������)�� ����
		_double Ratio = (TrackPosition - m_KeyFrames[m_iCurrentKeyFrame].Time) /
			(m_KeyFrames[m_iCurrentKeyFrame + 1].Time - m_KeyFrames[m_iCurrentKeyFrame].Time);
		
		_vector vSourScale, vDestScale;
		_vector vSourRotation, vDestRotation;
		_vector vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrame].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrame].vPosition);

		vDestScale = XMLoadFloat3(&LastKeyFrame.vScale);
		vDestRotation = XMLoadFloat4(&LastKeyFrame.vRotation);
		vDestPosition = XMLoadFloat3(&LastKeyFrame.vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);

		//���� Ű������ ����
		XMStoreFloat3(&m_CurrentKeyFrame[m_iCurrentKeyFrame].vScale, vScale);
		XMStoreFloat4(&m_CurrentKeyFrame[m_iCurrentKeyFrame].vRotation, vRotation);
		XMStoreFloat3(&m_CurrentKeyFrame[m_iCurrentKeyFrame].vPosition, vPosition);

		//�ش� ���� ���� ����� ��Ʈ�� 000����
		if (!strcmp(m_pBone->GetName(), "Bip001"))
		{
			if (!m_bSetPivot)
			{
				m_bSetPivot = true;
				m_vPrevBonePos = m_vIdleOriginPos;
			}

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
			_vector vFinalPos = vCurrentPos + vAlignPos;
			pTransform->Set_State(CTransform::STATE_POSITION, vFinalPos);

			//���� Ű�������� �� ��ġ ����
			XMStoreFloat3(&m_vPrevBonePos, vPosition);

			//ù��° �������� ��ġ�� �ִϸ��̼� ��Ʈ�� ����
			vPosition = XMLoadFloat3(&m_vIdleOriginPos);
		}
	}

	// Ű�������� ���¸� ������ SRT�� �����Ͽ� �� ��� ������Ʈ
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pBone->SetTransformationMatrix(TransformationMatrix);
}

_bool CChannel::InvalidateTransformLerp(_double TrackPosition, _double Duration, CTransform * pTransform, vector<KEY_FRAME> CurrentKeyFrame)
{
	_vector vScale;
	_vector vRotation;
	_vector vPosition;
	
	KEY_FRAME FirstKeyFrame = m_KeyFrames.front();
 	if (Duration - TrackPosition <= FirstKeyFrame.Time)
	{
		//�ִϸ��̼��� �����ٸ� ���� �������� ���¸� ����
		vScale = XMLoadFloat3(&FirstKeyFrame.vScale);
		vRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
		vPosition = XMLoadFloat3(&FirstKeyFrame.vPosition);

		//���� �������϶� �Ǻ� ������ �ʱ�ȭ
		if (!strcmp(m_pBone->GetName(), "Bip001"))
		{
			m_vPrevBonePos;
			XMStoreFloat3(&m_vPrevBonePos, vPosition);
			vPosition = XMLoadFloat3(&m_vIdleOriginPos);
		}
		
		m_bLerpFinish = true;
		Reset();

		return true;
	}
	else
	{
		//�ִϸ��̼��� ������� �ʾҰ� Ʈ���� ��ġ�� ������������ �ð��� �������� ��� ���������� ����
		while (TrackPosition >= m_CurrentKeyFrame[m_iCurrentKeyFrame + 1].Time)
			++m_iCurrentKeyFrame;

		//���� Ʈ���� ���൵�� (����������)0.f ~ 1.f(����������)�� ����
		//_double Ratio = (TrackPosition - NextKeyFrame.Time) /
		//	(NextKeyFrame.Time - NextKeyFrame.Time);
		_double Ratio = 0.2f;

		_vector vSourScale, vDestScale;
		_vector vSourRotation, vDestRotation;
		_vector vSourPosition, vDestPosition;
		
		vSourScale = XMLoadFloat3(&CurrentKeyFrame[m_iCurrentKeyFrame].vScale);
		vSourRotation = XMLoadFloat4(&CurrentKeyFrame[m_iCurrentKeyFrame].vRotation);
		vSourPosition = XMLoadFloat3(&CurrentKeyFrame[m_iCurrentKeyFrame].vPosition);

		vDestScale = XMLoadFloat3(&FirstKeyFrame.vScale);
		vDestRotation = XMLoadFloat4(&FirstKeyFrame.vRotation);
		vDestPosition = XMLoadFloat3(&FirstKeyFrame.vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (_float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (_float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (_float)Ratio);

		//���� Ű������ ����
		XMStoreFloat3(&CurrentKeyFrame[m_iCurrentKeyFrame].vScale, vScale);
		XMStoreFloat4(&CurrentKeyFrame[m_iCurrentKeyFrame].vRotation, vRotation);
		XMStoreFloat3(&CurrentKeyFrame[m_iCurrentKeyFrame].vPosition, vPosition);

		//�ش� ���� ���� ����� ��Ʈ�� 000����
		if (!strcmp(m_pBone->GetName(), "Bip001"))
		{
			if (!m_bSetPivot)
			{
				m_bSetPivot = true;
				m_vPrevBonePos = m_vIdleOriginPos;
			}

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
			_vector vFinalPos = vCurrentPos + vAlignPos;
			pTransform->Set_State(CTransform::STATE_POSITION, vFinalPos);

			//���� Ű�������� �� ��ġ ����
			XMStoreFloat3(&m_vPrevBonePos, vPosition);

			//ù��° �������� ��ġ�� �ִϸ��̼� ��Ʈ�� ����
			vPosition = XMLoadFloat3(&m_vIdleOriginPos);
		}
	}

	// Ű�������� ���¸� ������ SRT�� �����Ͽ� �� ��� ������Ʈ
	_matrix TransformationMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);
	m_pBone->SetTransformationMatrix(TransformationMatrix);

	return false;
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