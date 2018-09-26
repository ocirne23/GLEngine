#pragma once

#include "Graphics/IVertexAttributes.h"
#include "EASTL/vector.h"

class GLVertexAttributes : public IVertexAttributes
{
public:

	void initialize();
	virtual void addVertexAttribute(EVertexAttributeFormat format, uint numElements, bool normalize = false) override;
	virtual void reset() override;
	virtual void bind(IBuffer& vertexBuffer) override;

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

private:

	friend class GLContext;
	GLVertexAttributes() {}
	~GLVertexAttributes() override {}
	GLVertexAttributes(const GLVertexAttributes&) = delete;
};