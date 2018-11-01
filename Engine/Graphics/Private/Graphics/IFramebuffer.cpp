#include "Graphics/IFramebuffer.h"

#include "GL/GLFramebuffer.h"

IFramebuffer::IFramebuffer()
	: m_impl(make_pimpl<GLFramebuffer>())
{

}