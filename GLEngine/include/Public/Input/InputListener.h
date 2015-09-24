#pragma once

#include "GLEngine.h"
#include "Input/Input.h"

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
		if (m_func)
			return m_func(args...);
		else
			return RetType(0);
	}

private:

	typedef Tag TagType;
	std::function<RetType(Args...)> m_func;
};