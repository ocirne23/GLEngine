#pragma once

#include "Utils\VecForward.h"
#include "Utils\MatForward.h"

#include "rde\hash_map.h"
#include "rde\vector.h"
#include "rde\rde_string.h"

#include "Graphics\GL\GLTypes.h"

class GLShader
{
public:
	GLShader();
	~GLShader();
	GLShader(const GLShader& copy) = delete;

	void initialize(const char* vertexShaderPath, const char* fragShaderPath, const char* versionStr, const rde::vector<rde::string>* defines = NULL);
	void reloadProgram();

	void begin();
	void end();
	bool isBegun() const;
	unsigned int getID()const;

	//TODO: moar uniforms

	void setUniform1i(const char* uniformName, int val);
	void setUniform2i(const char* uniformName, const glm::ivec2& vec);
	void setUniform1f(const char* uniformName, float val);
	void setUniform2f(const char* uniformName, const glm::vec2& vec);
	void setUniform2fv(const char* uniformName, unsigned int count, const glm::vec2* vecs);
	void setUniform3f(const char* uniformName, const glm::vec3& vec);
	void setUniformMatrix4f(const char* uniformName, const glm::mat4& mat);
	void setUniformMatrix3f(const char* uniformName, const glm::mat3& mat);

	struct StrHash //BKDR hash algorithm
	{
		int operator() (const char* str) const
		{
			const int seed = 131;
			int hash = 0;
			while (*str)
			{
				hash = (hash * seed) + (*str);
				str++;
			}
			return hash & (0x7FFFFFFF);
		}
	};
	typedef rde::hash_map<const char*, GLint, StrHash> UniformLocMap;
	GLuint m_shaderID;
	UniformLocMap m_uniformLocMap;

private:

	/* Global state to avoid mixing multiple shaders. */
	static bool s_begun;

	bool m_initialized;

	bool m_begun;
	rde::string m_vertPath;
	rde::string m_fragPath;
	rde::string m_versionStr;
	rde::vector<rde::string> m_defines;
};