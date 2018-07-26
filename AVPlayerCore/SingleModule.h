#pragma once

template<class T>
class CSingleModule
{
public:
	CSingleModule<T>() = default;
	virtual ~CSingleModule<T>() = default;

	static T& getSingleModule()
	{
		return m_single;
	}

	static T* getSingleModulePtr()
	{
		return &m_single;
	}

private:
	CSingleModule<T>& operator=(const CSingleModule<T>&) = delete;

private:
	static T& m_single;
};
