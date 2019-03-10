#include "CoreAPI.h"

#pragma warning(disable : 4251) // needs to have dll-interface to be used by clients of class 

export module Core.Pimpl;

import std.memory;

export {

template<class T, class Deleter = void(*)(T*)>
class CORE_API Pimpl 
{
public:

	template<class U, class D>
	Pimpl(U *u, D&& deleter) noexcept
		: m_ptr(u, std::forward<D>(deleter))
	{}

	Pimpl(Pimpl&&) noexcept = default;

	T* operator->() const { return m_ptr.get(); }
	T& operator*() const  { return *m_ptr; }

	static void defaultDelete(T *p) noexcept { delete p; }

private:

	std::unique_ptr<T, Deleter> m_ptr;
};

template<class T, class... Args>
inline Pimpl<T> CORE_API make_pimpl(Args&&... args)
{
	return Pimpl<T>(new T(std::forward<Args>(args)...), &Pimpl<T>::defaultDelete);
}

} // export