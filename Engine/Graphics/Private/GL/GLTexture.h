#pragma once

#include "Graphics/ITexture.h"

class GLTexture final : public ITexture
{
public:

	GLTexture(ETextureType type);
	virtual ~GLTexture();

	virtual void setOptions(const ITextureSettings& options) override;

	uint getID() const { return m_id; }

private:

	void setupTextureStorage(const ITextureSettings& options);

private:

	uint m_id = 0;
	ETextureType m_type;
	ITextureSettings m_settings;
};