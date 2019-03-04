#pragma once

#include "Graphics/ITexture.h"

class GLTexture final : public ITexture
{
public:

	GLTexture(ETextureType type);
	virtual ~GLTexture();

	virtual void setOptions(const TextureSettings& options) override;
	virtual void loadTexture(void* data, ETexturePixelDataFormat format) override;

	uint getID() const { return m_id; }

private:

	void setupTextureStorage(const TextureSizeSettings& options);

private:

	uint m_id = 0;
	ETextureType m_type;
	TextureSettings m_settings;
};