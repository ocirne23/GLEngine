#pragma once

class IFramebuffer
{
public:

protected:
	friend class IContext;
	virtual ~IFramebuffer() {}
};