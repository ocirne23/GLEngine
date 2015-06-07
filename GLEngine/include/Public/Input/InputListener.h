#pragma once

#include "GLEngine.h"
#include "3rdparty/rde/vector.h"

#include <functional>

template <typename Tag, typename RetType, typename... Args>
class InputListener
{
public:

	InputListener()
	{
		GLEngine::input->getListeners(*this).push_back(this);
	}

	InputListener(std::function<RetType(Args...)> a_func) : m_func(a_func)
	{
		GLEngine::input->getListeners(*this).push_back(this);
	}

	~InputListener()
	{
		// unordered?
		GLEngine::input->getListeners(*this).remove_unordered(this);
	}

	void setFunc(std::function<RetType(Args...)> a_func)
	{
		m_func = a_func;
	}

	RetType call(Args... args)
	{
		return m_func(args...);
	}

private:
	typedef Tag TagType;
	std::function<RetType(Args...)> m_func;
};