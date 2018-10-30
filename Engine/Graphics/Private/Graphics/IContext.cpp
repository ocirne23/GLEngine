#include "Graphics/IContext.h"

#include "GL/GLContext.h"

IContext::IContext()
	: m_impl(new GLContext())
{

}

IContext::IContext(IContext&& move)
{
	m_impl = move.m_impl;
	move.m_impl = nullptr;
}