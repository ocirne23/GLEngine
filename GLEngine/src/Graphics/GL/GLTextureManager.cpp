#include "Graphics\GL\GLTextureManager.h"

#include "Graphics\GL\GL.h"
#include "Graphics\GL\Core\GLTexture.h"
#include "Utils\FileHandle.h"
#include "rde\rde_string.h"

GLTextureManager::GLTextureManager()
{

}

GLTextureManager::~GLTextureManager()
{

}

GLTextureManager::TextureHandle GLTextureManager::getTextureHandle(const rde::string& a_filePath)
{
	auto at = m_handleMap.find(a_filePath);
	if (at != m_handleMap.end())
	{
		return at->second.second;
	}
	else
	{
		GLTexture* texture = new GLTexture();
		texture->initialize(FileHandle(a_filePath));
		assert(texture->isLoaded());

		TextureHandle handle;
		handle.handle = 0;
#if 0
		handle.handle = glGetTextureHandleARB(texture->getTextureID());
		glMakeTextureHandleResidentARB(handle.handle);
		m_handleMap.insert(rde::make_pair(filePath, rde::make_pair(texture, handle)));
#endif

		return handle;
	}
}