#include "Graphics/GL/Wrappers/GLShader.h"

#include "Core.h"

#include "GLEngine.h"
#include "Graphics/GL/GL.h"
#include "Graphics/Utils/CheckGLError.h"
#include "Utils/FileHandle.h"
#include "EASTL/string.h"

#include <glm/glm.hpp>

bool GLShader::s_begun = false;

BEGIN_UNNAMED_NAMESPACE()

eastl::string getVersionStr()
{
	return eastl::string("#version 420\n");
}

eastl::string processIncludes(const eastl::string& a_str)
{
	eastl::string src = a_str;
	static const char* INCLUDE_STR = "#include \"";
	static const uint INCLUDE_STR_LEN = (uint) strlen(INCLUDE_STR);
	auto includePos = src.find(INCLUDE_STR);
	while (includePos != src.npos)
	{
		auto includeStartPos = includePos + INCLUDE_STR_LEN;
		auto includeEndPos = src.find("\"", includeStartPos);
		uint numChars = includeEndPos - includeStartPos;
		const eastl::string path = src.substr(includeStartPos, numChars);
		const eastl::string includedFileContents = FileHandle(path).readString();
		const eastl::string toReplace = "#include \"" + path + "\"";
		src.replace(src.find(toReplace), toReplace.length(), includedFileContents);
		includePos = src.find(INCLUDE_STR);
	}
	return src;
}

void checkProgramForErrors(const GLuint a_program)
{
	int success;
	glGetProgramiv(a_program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		print("Shader program was not linked!\n");
		assert(false);
	}
	glGetProgramiv(a_program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetProgramiv(a_program, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength)
		{
			eastl::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(a_program, maxLength, &maxLength, &infoLog[0]);
			print("link error: %s \n", &infoLog[0]);
		}
		else
			print("link error, no info log available\n");
		assert(false);
	}
	glValidateProgram(a_program);
	glGetProgramiv(a_program, GL_VALIDATE_STATUS, &success);
	if (success != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetProgramiv(a_program, GL_INFO_LOG_LENGTH, &maxLength);
		if (maxLength)
		{
			eastl::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(a_program, maxLength, &maxLength, &infoLog[0]);
			print("Validation error: %s \n", &infoLog[0]);
		}
		else
			print("Validation error, no info log available\n");
		assert(false);
	}
	if (!glIsProgram(a_program))
	{
		print("failed to create shader!\n");
		assert(false);
	}
	CHECK_GL_ERROR();
}

void checkShaderForErrors(const char* a_filePath, const GLuint a_shader, const GLenum a_shaderType, const char* a_source)
{
	GLint logLen;
	glGetShaderiv(a_shader, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 1)
	{
		char buffer[4096];
		glGetShaderInfoLog(a_shader, sizeof(buffer), NULL, buffer);
		print("Error in %s shader: %s : %s \n", a_filePath, buffer, a_source);
		assert(false);
	}
	int compileStatus;
	glGetShaderiv(a_shader, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		print("shader failed to compile \n");
		assert(false);
	}
	CHECK_GL_ERROR();
}

eastl::string preprocessShaderFile(const char* a_shaderFilePath, const eastl::vector<eastl::string>* a_defines, 
	                               const eastl::vector<eastl::string>* a_extensions)
{
	eastl::string contents = getVersionStr();					// #version xxx at top
	if (a_defines)												// add user defines
		for (const eastl::string& str : *a_defines)
			contents.append("#define ").append(str).append("\n");
	if (a_extensions)											// add user extensions
		for (const eastl::string& str : *a_extensions)
			contents.append("#extension ").append(str).append(" : require\n");
	
	FileHandle shaderFile(a_shaderFilePath);
	assert(shaderFile.exists());
	contents.append(shaderFile.readString());					// add shader data from file
	contents = processIncludes(contents);						// add all the included files
	return contents;
}

END_UNNAMED_NAMESPACE()

GLShader::~GLShader()
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);
}

void GLShader::initialize(const char* a_vertexShaderFilePath, const char* a_fragmentShaderFilePath,
	                      const eastl::vector<eastl::string>* a_defines, const eastl::vector<eastl::string>* a_extensions)
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);

	const GLuint program = glCreateProgram();
	assert(program);

	const GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	const GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	assert(vertexShader && fragmentShader);

	const eastl::string vertexShaderStr = preprocessShaderFile(a_vertexShaderFilePath, a_defines, a_extensions);
	const eastl::string fragmentShaderStr = preprocessShaderFile(a_fragmentShaderFilePath, a_defines, a_extensions);
	const char* vertexShaderSource = vertexShaderStr.c_str();
	const char* fragmentShaderSource = fragmentShaderStr.c_str();
	//print("Vertex shader:\n%s", vertexShaderSource);
	//print("Fragment shader:\n%s", fragmentShaderSource);

	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	checkShaderForErrors(a_vertexShaderFilePath, vertexShader, GL_VERTEX_SHADER, vertexShaderSource);
	glAttachShader(program, vertexShader);
	glDeleteShader(vertexShader);

	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	checkShaderForErrors(a_fragmentShaderFilePath, fragmentShader, GL_FRAGMENT_SHADER, fragmentShaderSource);
	glAttachShader(program, fragmentShader);
	glDeleteShader(fragmentShader);

	glLinkProgram(program);
	checkProgramForErrors(program);

	m_shaderID = program;
	m_initialized = true;
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
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
		m_uniformLocMap.insert(eastl::make_pair(a_uniformName, loc));
		glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
	else
		glUniformMatrix3fv(it->second, 1, GL_FALSE, &mat[0][0]);
}