#pragma once

#include "Graphics/IVertexAttributes.h"
#include "EASTL/vector.h"

class GLVertexBuffer;

class GLVertexAttributes : public IVertexAttributes
{
public:

	void initialize();
	virtual void addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize) override;
	virtual void reset() override;

	void bind(GLVertexBuffer& buffer);

private:

	struct GLVertexAttribute
	{
		GLVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize) 
			: format(format)
			, numElements(numElements)
			, normalize(normalize) 
		{
		}

		EVertexAttributeFormat format = EVertexAttributeFormat::FLOAT;
		uint numElements = 0;
		bool normalize = false;
	};

	bool m_initialized = false;
	uint m_vaoID = 0;
	eastl::vector<GLVertexAttribute> m_attributes;
};