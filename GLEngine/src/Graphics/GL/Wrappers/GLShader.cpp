#include "Graphics/GL/Wrappers/GLShader.h"

#include "Core.h"

#include "GLEngine.h"

#include "Graphics/GL/GL.h"
#include "Graphics/GL/GLVars.h"

#include "3rdparty/rde/rde_string.h"
#include "Utils/FileHandle.h"

#include <glm/glm.hpp>

bool GLShader::s_begun = false;

BEGIN_UNNAMED_NAMESPACE()

rde::string getVersionStr()
{
	return rde::string("#version 330\n");
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

void checkProgramForErrors(const GLuint a_program)
{
	int success;
	glGetProgramiv(a_program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		print("Shader program was not linked.!\n");
	}

	glGetProgramiv(a_program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetProgramiv(a_program, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength)
		{
			rde::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(a_program, maxLength, &maxLength, &infoLog[0]);
			print("link error: %s \n", &infoLog[0]);
		}
		else
		{
			print("link error, no info log available \n");
		}
	}

	glValidateProgram(a_program);
	glGetProgramiv(a_program, GL_VALIDATE_STATUS, &success);
	if (success != GL_TRUE)
		print("Shader program was not validated.!\n");

	if (!glIsProgram(a_program))
		print("failed to create shader \n");
}

void checkShaderForErrors(const GLuint a_shader, const GLenum a_shaderType, const char* a_source)
{
	GLint logLen;
	glGetShaderiv(a_shader, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 1)
	{
		char buffer[4096];
		glGetShaderInfoLog(a_shader, sizeof(buffer), NULL, buffer);
		const char* typeString;
		switch (a_shaderType)
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
	glGetShaderiv(a_shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		print("shader failed to compile \n");
	}
}

rde::string preprocessShaderFile(const char* a_shaderFilePath, const rde::vector<rde::string>* a_defines, const rde::vector<rde::string>* a_extensions)
{
	rde::string contents = getVersionStr();						// #version xxx at top
	contents.append(getSystemDefines());						// add generic defines
	if (a_defines)												// add user defines
		for (const rde::string& str : *a_defines)
			contents.append("#define ").append(str).append("\n");
	if (a_extensions)											// add user extensions
		for (const rde::string& str : *a_extensions)
			contents.append("#extension ").append(str).append(" : require\n");
	
	FileHandle shaderFile(a_shaderFilePath);
	assert(shaderFile.exists());
	contents.append(shaderFile.readString());					// add shader data from file
	contents = processIncludes(contents);						// add all the included files
	return contents;
}

void attachShaderSource(GLuint a_prog, GLenum a_type, const char* a_source)
{
	const GLuint shader = glCreateShader(a_type);
	assert(shader);

	glShaderSource(shader, 1, &a_source, NULL);
	glCompileShader(shader);

	checkShaderForErrors(shader, a_type, a_source);

	glAttachShader(a_prog, shader);
	glDeleteShader(shader);
}

END_UNNAMED_NAMESPACE()

GLShader::~GLShader()
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);
}

void GLShader::initialize(const char* a_vertexShaderFilePath, const char* a_fragmentShaderFilePath,
						  const rde::vector<rde::string>* a_defines, const rde::vector<rde::string>* a_extensions)
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);

	const GLuint program = glCreateProgram();
	assert(program);

	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	assert(vertexShader && fragmentShader);

	const rde::string vertexShaderStr = preprocessShaderFile(a_vertexShaderFilePath, a_defines, a_extensions);
	const rde::string fragmentShaderStr = preprocessShaderFile(a_fragmentShaderFilePath, a_defines, a_extensions);
	const char* vertexShaderSource = vertexShaderStr.c_str();
	const char* fragmentShaderSource = fragmentShaderStr.c_str();
	print("Vertex shader:\n%s", vertexShaderSource);
	print("Fragment shader:\n%s", fragmentShaderSource);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	checkShaderForErrors(vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
	glAttachShader(program, vertexShader);
	glDeleteShader(vertexShader);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderForErrors(fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSource);
	glAttachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	glLinkProgram(program);
	checkProgramForErrors(program);

	m_shaderID = program;
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
void GLShader::setUniform1i(const char* a_uniformName, int a_val)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniform1i(loc, a_val);
	}
	else
		glUniform1i(it->second, a_val);
}

void GLShader::setUniform1f(const char* a_uniformName, float a_val)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniform1f(loc, a_val);
	}
	else
		glUniform1f(it->second, a_val);
}

void GLShader::setUniform2f(const char* a_uniformName, const glm::vec2& a_vec)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniform2f(loc, a_vec.x, a_vec.y);
	}
	else
		glUniform2f(it->second, a_vec.x, a_vec.y);
}

void GLShader::setUniform2i(const char* a_uniformName, const glm::ivec2& a_vec)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniform2i(loc, a_vec.x, a_vec.y);
	}
	else
		glUniform2i(it->second, a_vec.x, a_vec.y);
}
void GLShader::setUniform2fv(const char* a_uniformName, uint a_count, const glm::vec2* a_vecs)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniform2fv(loc, a_count, &a_vecs[0][0]);
	}
	else
		glUniform2fv(it->second, a_count, &a_vecs[0][0]);
}

void GLShader::setUniform3f(const char* a_uniformName, const glm::vec3& a_vec)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniform3f(loc, a_vec.x, a_vec.y, a_vec.z);
	}
	else
		glUniform3f(it->second, a_vec.x, a_vec.y, a_vec.z);
}
void GLShader::setUniformMatrix4f(const char* a_uniformName, const glm::mat4& a_mat)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniformMatrix4fv(loc, 1, GL_FALSE, &a_mat[0][0]);
	}
	else
		glUniformMatrix4fv(it->second, 1, GL_FALSE, &a_mat[0][0]);
}
void GLShader::setUniformMatrix3f(const char* a_uniformName, const glm::mat3& mat)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(a_uniformName);
	if (it == m_uniformLocMap.end())
	{
		const GLint loc = glGetUniformLocation(m_shaderID, a_uniformName);
		m_uniformLocMap.insert(rde::make_pair(a_uniformName, loc));
		glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
	else
		glUniformMatrix3fv(it->second, 1, GL_FALSE, &mat[0][0]);
}