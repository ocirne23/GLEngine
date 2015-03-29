#pragma once

#include "Core.h"

#include "Utils/FileHandle.h"
#include "Utils/MatForward.h"
#include "Utils/VecForward.h"

#include "rde/hash_map.h"
#include "rde/vector.h"
#include "rde/rde_string.h"

class FileHandle;

class GLShader
{
public:
	GLShader();
	~GLShader();
	GLShader(const GLShader& copy) = delete;

	void initialize(const FileHandle& vertexShaderFile, const FileHandle& fragmentShaderFile,
	                const rde::vector<rde::string>* defines = NULL, const rde::vector<rde::string>* extensions = NULL);

	void begin();
	void end();
	bool isBegun() const { return m_begun; }
	uint getID() const { return m_shaderID; }

	void setUniform1i(const char* uniformName, int val);
	void setUniform2i(const char* uniformName, const glm::ivec2& vec);
	void setUniform1f(const char* uniformName, float val);
	void setUniform2f(const char* uniformName, const glm::vec2& vec);
	void setUniform2fv(const char* uniformName, uint count, const glm::vec2* vecs);
	void setUniform3f(const char* uniformName, const glm::vec3& vec);
	void setUniformMatrix4f(const char* uniformName, const glm::mat4& mat);
	void setUniformMatrix3f(const char* uniformName, const glm::mat3& mat);

	typedef rde::hash_map<const char*, int> UniformLocMap;

private:

	bool m_initialized = false;
	bool m_begun       = false;
	uint m_shaderID    = 0;
	UniformLocMap m_uniformLocMap;

private:

	/* Global state to avoid mixing multiple shaders. */
	static bool s_begun;
};