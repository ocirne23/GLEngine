#pragma once

#include "EASTL/vector.h"

class GLVertexAttributes
{
public:

	void initialize();
	void addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize = false);
	void reset();
	void bind(IBuffer& vertexBuffer);

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