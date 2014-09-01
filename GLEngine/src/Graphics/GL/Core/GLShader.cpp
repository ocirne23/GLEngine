#include "Graphics\GL\Core\GLShader.h"

#include "Core.h"
#include "Utils\FileHandle.h"
#include "rde\rde_string.h"
#include "Graphics\GL\GL.h"

#include <glm\glm.hpp>

bool GLShader::s_begun = false;

static const char* INCLUDE_STR = "#include \"";
static const uint INCLUDE_STR_LEN = (uint) strlen(INCLUDE_STR);

//#define SHADER_STRICT_UNIFORM_LOC

BEGIN_UNNAMED_NAMESPACE()

rde::string processIncludes(const rde::string& str)
{
	rde::string src = str;

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

rde::string appendAfterVersion(const rde::string& str, const rde::vector<rde::string>* defines, const rde::vector<rde::string>* extensions)
{
	auto version = str.find("#version");
	auto at = str.find("\n", version) + 1; // find first newline after #version
	rde::string first = str.substr(0, at);
	rde::string last = str.substr(at, str.length());

	if (defines)
	{
		for (const rde::string& define : *defines)
		{
			first += rde::string("#define ");
			first += define;
			first += rde::string("\n");
		}
	}
	if (extensions)
	{
		for (const rde::string& define : *extensions)
		{
			first += rde::string("#extension ");
			first += define;
			first += rde::string(" : require\n");
		}
	}
	first += last;

	return first;
}

void attachShaderSource(GLuint prog, GLenum type, const char * source)
{
	GLuint sh = glCreateShader(type);

	if (!sh)
	{
		print("Could not create shader %i \n", type);
	}

	glShaderSource(sh, 1, &source, NULL);
	glCompileShader(sh);

	GLint logLen;
	glGetShaderiv(sh, GL_INFO_LOG_LENGTH, &logLen);
	if (logLen > 1)
	{
		char buffer[4096];
		glGetShaderInfoLog(sh, sizeof(buffer), NULL, buffer);
		const char* typeString;
		switch (type) {
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
		print("Error in %s shader: %s : %s \n", typeString, buffer, source);
	}
	int compileStatus;
	glGetShaderiv(sh, GL_COMPILE_STATUS, &compileStatus);
	if (compileStatus != GL_TRUE)
	{
		printf("shader failed to compile \n");
	}

	glAttachShader(prog, sh);
	glDeleteShader(sh);
}

GLuint createShaderProgram(const FileHandle& vertexShaderFile, const FileHandle& fragmentShaderFile, rde::string versionStr,
	const rde::vector<rde::string>* defines, const rde::vector<rde::string>* extensions)
{
	GLuint program = glCreateProgram();

	{
		rde::string contents = rde::string("#version ").append(versionStr).append("\n").append(vertexShaderFile.readString());
		contents = processIncludes(contents);
		contents = appendAfterVersion(contents, defines, extensions);
		attachShaderSource(program, GL_VERTEX_SHADER, contents.c_str());
	}
	{
		rde::string contents = rde::string("#version ").append(versionStr).append("\n").append(fragmentShaderFile.readString());
		contents = processIncludes(contents);
		contents = appendAfterVersion(contents, defines, extensions);
		attachShaderSource(program, GL_FRAGMENT_SHADER, contents.c_str());
	}

	glLinkProgram(program);

	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		printf("Shader program was not linked.!\n");
	}

	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if (success != GL_TRUE)
	{
		GLint maxLength = 0;
		glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

		rde::vector<GLchar> infoLog(maxLength);
		glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
		printf("link error: %s \n", &infoLog[0]);
	}

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &success);
	if (success != GL_TRUE)
	{
		printf("Shader program was not validated.!\n");
	}

	if (!glIsProgram(program))
	{
		printf("failed to create shader \n");
	}

	return program;
}
END_UNNAMED_NAMESPACE()

GLShader::GLShader()
: m_shaderID(0)
, m_begun(false)
{
}

GLShader::~GLShader()
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);
}

void GLShader::initialize(const FileHandle& vertexShaderFile, const FileHandle& fragmentShaderFile, const rde::string& versionStr, 
	const rde::vector<rde::string>* defines, const rde::vector<rde::string>* extensions)
{
	if (m_shaderID)
		glDeleteProgram(m_shaderID);

	m_shaderID = createShaderProgram(vertexShaderFile, fragmentShaderFile, rde::string(versionStr), defines, extensions);
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
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC
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
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC
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
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC		
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
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC		
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniform2i(loc, vec.x, vec.y);
	}
	else
		glUniform2i(it->second, vec.x, vec.y);
}
void GLShader::setUniform2fv(const char* uniformName, unsigned int count, const glm::vec2* vecs)
{
	assert(m_begun);
	auto it = m_uniformLocMap.find(uniformName);
	if (it == m_uniformLocMap.end())
	{
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC		
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
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC		
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
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC		
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
		GLint loc = glGetUniformLocation(m_shaderID, uniformName);
#ifdef SHADER_STRICT_UNIFORM_LOC
		assert(loc != -1);
#endif //SHADER_STRICT_UNIFORM_LOC		
		m_uniformLocMap.insert(rde::make_pair(uniformName, loc));
		glUniformMatrix3fv(loc, 1, GL_FALSE, &mat[0][0]);
	}
	else
		glUniformMatrix3fv(it->second, 1, GL_FALSE, &mat[0][0]);
}