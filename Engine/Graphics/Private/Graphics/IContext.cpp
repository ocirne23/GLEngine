#include "Graphics/IContext.h"

#include "GL/GLContext.h"

IContext::IContext(const EContextType& type)
	: m_impl(make_pimpl<GLContext>())
{
	assert(type == EContextType::OPENGL);
}