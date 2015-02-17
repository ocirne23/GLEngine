#include "Graphics/GL/Core/GLShader.h"

#include "Core.h"

#include "GLEngine.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLVars.h"

#include "rde/rde_string.h"
#include "Utils/FileHandle.h"

#include <glm/glm.hpp>

bool GLShader::s_begun = false;

// if an error should be printed when an uniform is set which isn't present in the shader.
// #define SHADER_STRICT_UNIFORM_LOC

BEGIN_UNNAMED_NAMESPACE()

rde::string getVersionStr()
{
	return rde::string("#version 330 core\n");
}

rde::string getSystemDefines()
{
	rde::string str;
	if (rde::string(GLVars::getVendorStr()).find("NVIDIA") != rde::string::npos
		&& GLVars::getGLMajorVersion() >= 4
		&& GLVars::getGLMinorVersion() >= 2)
	{
		str.append("#define ").append(rde::string("GLE_DYNAMIC_INDEXING")).append("\n");
	}
	return str;
}

rde::string processIncludes(const rde::string& a_str)
{
	rde::string src = a_str;

	static const char* INCLUDE_STR = "#include \"";
	static const uint INCLUDE_STR_LEN = (uint) strlen(INCLUDE_STR);

	auto includePos = src.find(INCLUDE_STR);
	while (includePos != src.npos)
	{
		auto fileStartPos = includePos + INCLUDE_STR_LEN;
		auto fileEndPos = src.find("\"", fileStartPos) + fileStartPos;
		rde::string path = src.substr(fileStartPos, fileEndPos);
		src = src.substr(0, includePos).append(FileHandle(path.c_str()).readString()).append(src.substr(fileEndPos + 1, src.length()));
		auto next = src.find(INCLUDE_STR, includePos);
		includePos = next == src.npos ? next : next + includePos;
	}

	return src;
} 

void attachShaderSource(GLuint a_prog, GLenum a_type, const char * a_source)
{
	const GLuint shader = glCreateShader(a_type);

	if (!shader)
		print("Could not create shader %i \n", a_type);

	glShaderSource(shader, 1, &a_source, NULL);
	glCompileShader(shader);

	GLint logLen;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 1)
	{
		char buffer[4096];
		glGetShaderInfoLog(shader, sizeof(buffer), NULL, buffer);
		const char* typeString;
		switch (a_type) 
		{
		case GL_FRAGMENT_SHADER:
			typeString = "fragment";
			break;
		case GL_VERTEX_SHADER:
			typeString = "vertex";
			break;
		default:
			typeString = "nan";
			break;
		}
		print("Error in %s shader: %s : %s \n", typeString, buffer, a_source);
	}
	int compileStatus;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		print("shader failed to compile \n");
	}

	glAttachShader(a_prog, shader);
	glDeleteShader(shader);
}

GLuint createShaderProgram(const FileHandle& a_vertexShaderFile, const FileHandle& a_fragmentShaderFile, 
	const rde::vector<rde::string>* a_defines, const rde::vector<rde::string>* a_extensions)
{
	const GLuint program = glCreateProgram();
	{
		rde::string contents = getVersionStr();
		contents.append(getSystemDefines());
		if (a_defines)
			for (const rde::string& str : *a_defines)
				contents.append("#define ").append(str).append("\n");
		if (a_extensions)
			for (const rde::string& str : *a_extensions)
				contents.append("#extension ").append(str).append(" : require\n");
		contents.append(a_vertexShaderFile.readString());
		contents = processIncludes(contents);
		attachShaderSource(program, GL_VERTEX_SHADER, contents.c_str());
	}
	{
		rde::string contents = getVersionStr();
		contents.append(getSystemDefines());
		if (a_defines)
			for (const rde::string& str : *a_defines)
				contents.append("#define ").append(str).append("\n");
		if (a_extensions)
			for (const rde::string& str : *a_extensions)
				contents.append("#extension ").append(str).append(" : require\n");
		contents.append(a_fragmentShaderFile.readString());
		contents = processIncludes(contents);
		attachShaderSource(program, GL_FRAGMENT_SHADER, contents.c_str());
	}

	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		print("Shader program was not linked.!\n");
	}

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		rde::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		print("link error: %s \n", &infoLog[0]);
	}

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if (success != GL_TRUE)
		print("Shader program was not validated.!\n");

	if (!glIsProgram(program))
		print("failed to create shader \n");

	return program;
}

void assertStrictUniformLoc(GLint loc)
{
#ifdef SHADER_STRICT_UNIFORM_LOC
	assert(loc != -1);
#endif // SHADER_STRICT_UNIFORM_LOC
}

END_UNNAMED_NAMESPACE()

GLShader::GLShader() : m_shaderID(0), m_begun(false) 
{
}

GLShader::~GLShader()
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);
}

void GLShader::initialize(const FileHandle& vertexShaderFile, const FileHandle& fragmentShaderFile, 
	const rde::vector<rde::string>* defines, const rde::vector<rde::string>* extensions)
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);

	m_shaderID = createShaderProgram(vertexShaderFile, fragmentShaderFile, defines, extensions);
}

void GLShader::begin()
{
	assert(!s_begun);
	assert(!m_begun);
	s_begun = true;
	m_begun = true;
	glUseProgram(m_shaderID);
}

void GLShader::end()
{
	assert(s_begun);
	assert(m_begun);
	s_begun = false;
	m_begun = false;
	glUseProgram(0);
}

//#define SHADER_STRICT_UNIFORM_LOC
void GLShader::setUniform1i(const char* uniformName, int val)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniform1i(loc, val);
	}
	else
		glUniform1i(it->second, val);
}

void GLShader::setUniform1f(const char* uniformName, float val)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniform1f(loc, val);
	}
	else
		glUniform1f(it->second, val);
}

void GLShader::setUniform2f(const char* uniformName, const glm::vec2& vec)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniform2f(loc, vec.x, vec.y);
	}
	else
		glUniform2f(it->second, vec.x, vec.y);
}

void GLShader::setUniform2i(const char* uniformName, const glm::ivec2& vec)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniform2i(loc, vec.x, vec.y);
	}
	else
		glUniform2i(it->second, vec.x, vec.y);
}
void GLShader::setUniform2fv(const char* uniformName, uint count, const glm::vec2* vecs)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniform2fv(loc, count, &vecs[0][0]);
	}
	else
		glUniform2fv(it->second, count, &vecs[0][0]);
}

void GLShader::setUniform3f(const char* uniformName, const glm::vec3& vec)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniform3f(loc, vec.x, vec.y, vec.z);
	}
	else
		glUniform3f(it->second, vec.x, vec.y, vec.z);
}
void GLShader::setUniformMatrix4f(const char* uniformName, const glm::mat4& mat)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniformMatrix4fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
	else
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &mat[0][0]);
}
void GLShader::setUniformMatrix3f(const char* uniformName, const glm::mat3& mat)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, uniformName);
		assertStrictUniformLoc(loc);
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
	else
		glUniformMatrix3fv(it->second, 1, GL_FALSE, &mat[0][0]);
}