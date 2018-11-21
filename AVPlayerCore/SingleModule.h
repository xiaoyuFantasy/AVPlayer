#pragma once

template<class T>
class CSingleModule
{
public:
	CSingleModule<T>() = default;
	virtual ~CSingleModule<T>() = default;

	static T& getSingleModule()
	{
		static T module;
		return module;
	}

	static T* getSingleModulePtr()
	{
		static T module;
		return &module;
	}

private:
	CSingleModule<T>& operator=(const CSingleModule<T>&) = delete;

private:

};
