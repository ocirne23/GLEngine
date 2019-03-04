#pragma once

#include "Core.h"
#include "Graphics/IFramebuffer.h"
#include "EASTL/vector.h"

class ITexture;

class GLFramebuffer : public IFramebuffer
{
public:

	GLFramebuffer();
	~GLFramebuffer();

	virtual void attachTexture(const ITexture& tex, ETextureAttachment attachment) override;
	virtual void bind(EFramebufferTarget target) override;

	

private:

	uint m_id = 0;
	EFramebufferTarget m_target = EFramebufferTarget::DRAWREAD;
	eastl::vector<uint> m_drawBuffers;
};