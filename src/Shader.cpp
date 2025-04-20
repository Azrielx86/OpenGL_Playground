//
// Created by kaguya on 6/17/24.
//

#include "Shader.h"
#include <cstring>
#include <format>
#include <fstream>
#include <iostream>

std::string Shader::ReadFile(const char *file)
{
	std::string stream, line;
	std::ifstream ifstream(file, std::ios::in);

	if (!ifstream.is_open())
		throw std::runtime_error(std::format("Can't open file: {}.", file));

	while (!ifstream.eof())
	{
		std::getline(ifstream, line);
		stream.append(line).append("\n");
	}

	return stream;
}

GLenum ShaderTypeConverter::ToGlenum(const ShaderType type)
{
	switch (type)
	{
	case VERTEX:
		return GL_VERTEX_SHADER;
	case FRAGMENT:
		return GL_FRAGMENT_SHADER;
	case GEOMETRY:
		return GL_GEOMETRY_SHADER;
	default:
		throw std::runtime_error(std::format("Unsupported shader type: {}", static_cast<unsigned int>(type)));
	}
}

ShaderType ShaderTypeConverter::FromExtension(const char *ext)
{
	if (strcmp(ext, "vert") == 0)
		return VERTEX;
	if (strcmp(ext, "frag") == 0)
		return FRAGMENT;
	if (strcmp(ext, "geom") == 0)
		return GEOMETRY;
	throw std::runtime_error(std::format("Unsupported shader extension: {}", ext));
}

void Shader::DestroyShader()
{
	if (programId != 0) glDeleteProgram(programId);
	programId = 0;
	isLinked = false;
}

void Shader::ReloadShader()
{
	if (programId == 0) return;
	DestroyShader();
	// LoadShader(vertexPath.c_str(), fragmentPath.c_str());
	CreateProgram();
	for (const auto &[type, path] : shadersAttached)
	{
		AttachShader(path.c_str(), static_cast<ShaderType>(type));
	}

	LinkProgram();
}

GLuint Shader::CompileShader(const GLuint program, const char *shaderCode, const GLenum type)
{
	const GLuint shader = glCreateShader(type);
	const GLchar *code[1];
	code[0] = shaderCode;

	GLint codeLen[1];
	codeLen[0] = static_cast<GLint>(strlen(shaderCode));

	glShaderSource(shader, 1, code, codeLen);
	glCompileShader(shader);

	GLint result;
	GLchar log[1024] = {};
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(shader, sizeof(log), nullptr, log);
		throw std::runtime_error(std::format("Error: {}", log));
	}

	glAttachShader(program, shader);
	return shader;
}

void Shader::Use() const { glUseProgram(programId); }

Shader &Shader::CreateProgram()
{
	programId = glCreateProgram();
	if (!programId)
		throw std::runtime_error("Canno't create GL program.");
	return *this;
}

Shader &Shader::AttachShader(const char *path, ShaderType shaderType)
{
	const GLenum type = ShaderTypeConverter::ToGlenum(shaderType);

	if ((shaderTypes & shaderType) == 0)
	{
		shaderTypes |= shaderType;
		shadersAttached.emplace_back(shaderType, path);
	}

	const auto code = ReadFile(path);

	CompileShader(programId, code.c_str(), type);
	return *this;
}

void Shader::LinkProgram()
{
	GLint result;
	GLchar log[1024] = {0};

	glLinkProgram(programId);
	glGetProgramiv(programId, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(programId, sizeof(log), nullptr, log);
		throw std::runtime_error(std::format("Error: {}", log));
	}

	glValidateProgram(programId);
	glGetProgramiv(programId, GL_VALIDATE_STATUS, &result);

	if (!result)
	{
		glGetProgramInfoLog(programId, sizeof(log), nullptr, log);
		throw std::runtime_error(std::format("Error: {}", log));
	}

	isLinked = true;
}

void Shader::Set(const char *name, const int value) const { glUniform1i(glGetUniformLocation(programId, name), value); }

void Shader::Set(const char *name, const bool value) const { glUniform1i(glGetUniformLocation(programId, name), static_cast<int>(value)); }

void Shader::Set(const char *name, const float value) const { glUniform1f(glGetUniformLocation(programId, name), value); }

void Shader::Set(const GLint id, const int value) const { glUniform1i(id, value); }

void Shader::Set(const GLint id, const bool value) const { glUniform1i(id, static_cast<int>(value)); }

void Shader::Set(const GLint id, const float value) const { glUniform1f(id, value); }

GLint Shader::GetUniformLocation(const char *name) const { return glGetUniformLocation(programId, name); }

GLuint Shader::GetProgramResourceIndex(const char *dataName) const
{
	return glGetProgramResourceIndex(programId, GL_SHADER_STORAGE_BLOCK, dataName);
}

void Shader::StorageBlockBinding(const unsigned int blockIndex, const unsigned int bindingPointIndex) const
{
	glShaderStorageBlockBinding(programId, blockIndex, bindingPointIndex);
}

bool Shader::IsLinked() const
{
	return isLinked;
}
