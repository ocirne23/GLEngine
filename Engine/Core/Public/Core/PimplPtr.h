#pragma once

#pragma warning(disable : 4251) // needs to have dll-interface to be used by clients of class 

#include <memory>

template<class T, class Deleter = void(*)(T*)>
class PimplPtr 
{
public:

	template<class U, class D>
	PimplPtr(U *u, D&& deleter) noexcept
		: m_ptr(u, std::forward<D>(deleter))
	{}

	PimplPtr(PimplPtr&&) noexcept = default;

	T* operator->() const { return m_ptr.get(); }
	T& operator*() const  { return *m_ptr; }

	static void defaultDelete(T *p) noexcept { delete p; }

private:

	std::unique_ptr<T, Deleter> m_ptr;
};

template<class T, class... Args>
inline PimplPtr<T> make_pimpl(Args&&... args) 
{
	return PimplPtr<T>(new T(std::forward<Args>(args)...), &PimplPtr<T>::defaultDelete);
}