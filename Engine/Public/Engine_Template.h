#pragma once

namespace Engine
{
	template<typename T>
	unsigned long Safe_AddRef(T& classType)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != classType)
			classType->AddRef();

		return dwRefCnt;
	}

	template<typename T>
	unsigned long Safe_Release(T& classType)
	{
		unsigned long dwRefCnt = 0;

		if (nullptr != classType)
		{
			dwRefCnt = classType->Release();

			if (0 == dwRefCnt)
			{
				classType = nullptr;
			}
		}

		return dwRefCnt;
	}

	template<typename T>
	void Safe_Delete(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete pointer;
			pointer = nullptr;
		}
	}

	template<typename T>
	void Safe_Delete_Array(T& pointer)
	{
		if (nullptr != pointer)
		{
			delete[] pointer;
			pointer = nullptr;
		}
	}

	class CTagFinder
	{
	public:
		explicit CTagFinder(const wchar_t* pTag)
			: m_pTargetTag(pTag) {}
		~CTagFinder() = default;

	public:
		template<typename T>
		bool operator()(const T& pair)
		{
			if (0 == lstrcmpW(m_pTargetTag, pair.first))
				return true;
			else
				return false;
		}

	private:
		const wchar_t* m_pTargetTag = nullptr;
	};
}