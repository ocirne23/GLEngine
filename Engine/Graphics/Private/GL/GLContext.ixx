#include "Core.h"
#include "GL.h"
#include "Platform/PlatformWindows.h"

export module Graphics.GLContext;

import Graphics.IContext;
import Graphics.IBuffer;
import Graphics.GLBuffer;
import Graphics.GLTexture;

struct SDL_Window;
typedef void* ContextHandle;

export class GLContext final : public IContext
{
public:

	GLContext(void* hdc)
	{
		assert(!m_glContext);
		m_glContext = WinPlatform::createHighestGLContext(hdc, 4, 5);
		m_glVendor = rcast<const char*>(glGetString(GL_VENDOR));
		m_glRenderer = rcast<const char*>(glGetString(GL_RENDERER));
		m_glDriverVersion = rcast<const char*>(glGetString(GL_VERSION));
		glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, rcast<GLint*>(&m_maxTextureUnits));
		glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE, rcast<GLint*>(&m_uboMaxSize));
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, rcast<GLint*>(&m_maxTextureSize));
		glGetIntegerv(GL_MAJOR_VERSION, rcast<GLint*>(&m_glMajorVersion));
		glGetIntegerv(GL_MINOR_VERSION, rcast<GLint*>(&m_glMinorVersion));
		glGetIntegerv(GL_MAX_SAMPLES, rcast<GLint*>(&m_maxMSAASamples));

		print("Created GL Context: %i.%i\n", m_glMajorVersion, m_glMinorVersion);
		print("Vendor: %s\n", m_glVendor.c_str());
		print("Renderer: %s\n", m_glRenderer.c_str());
		print("DriverVersion: %s\n", m_glDriverVersion.c_str());
		print("Max MSAA samples: %i\n", m_maxMSAASamples);
	}

	GLContext(const GLContext& copy) = delete;

	virtual ~GLContext() override
	{
		assert(m_glContext);
		WinPlatform::deleteContext(m_glContext);
	}

	virtual owner<IBuffer*> createBuffer(uint64 size, const EBufferUsageType& type) override
	{
		return 0;// new GLBuffer(size, type);
	}

	virtual void destroyBuffer(owner<IBuffer*>& buffer) override
	{
		delete static_cast<GLBuffer*>(buffer);
		buffer = nullptr;
	}

	virtual owner<ITexture*> createTexture(const ETextureType& type) override
	{
		return new GLTexture(type);
	}

	virtual void destroyTexture(owner<ITexture*>& texture) override
	{
		delete static_cast<GLTexture*>(texture);
		texture = nullptr;
	}

	virtual EContextType getContextType() const override 
	{ 
		return EContextType::OPENGL; 
	}

	uint                 getGLMajorVersion() const     { return m_glMajorVersion; }
	uint                 getGLMinorVersion() const     { return m_glMinorVersion; }
	uint                 getMaxTextureUnits() const    { return m_maxTextureUnits; }
	uint                 getUBOMaxSize() const         { return m_uboMaxSize; }
	const std::string& getVendorStr() const            { return m_glVendor; }
	const std::string& getRendererStr() const          { return m_glRenderer; }
	const std::string& getDriverVersionStr() const     { return m_glDriverVersion; }
	const ContextHandle  getSDLContext() const         { return m_glContext; }

private:


private:

	ContextHandle m_glContext = NULL;

	uint m_glMajorVersion = 0;
	uint m_glMinorVersion = 0;
	uint m_maxTextureUnits = 0;
	uint m_uboMaxSize = 0;
	uint m_maxTextureSize = 0;
	uint m_maxMSAASamples = 0;

	std::string m_glVendor;
	std::string m_glRenderer;
	std::string m_glDriverVersion;
};