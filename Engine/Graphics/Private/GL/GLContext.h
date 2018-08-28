#pragma once

#include "Graphics/IContext.h"

class GLContext : public IContext
{
public:

	virtual EContextType getType() const override { return EContextType::OPENGL; }
	virtual owner<IBuffer*> createBuffer(const EBufferType& type) override;
	virtual void destroyBuffer(owner<IBuffer*> buffer) override;

private:

	friend class Window;
	GLContext();
	~GLContext() override;
	GLContext(const GLContext&) = delete;
};