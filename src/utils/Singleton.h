#ifndef __UTILS_SINGLETON_H__
#define __UTILS_SINGLETON_H__

// A simple singleton template.
template <typename T>
class CSingleton
{
public:
	static T& Instance()
	{
		static T s_Instance;
		return s_Instance;
	}

protected:
	CSingleton() {}
	virtual ~CSingleton() {}
};

#endif