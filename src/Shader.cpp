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

void Shader::LoadShader(const char *vertexShader, const char *fragmentShader)
{
	if (!firstLoad)
	{
		vertexPath = vertexShader;
		fragmentPath = fragmentShader;
		firstLoad = true;
	}

	auto vertexStr = ReadFile(vertexShader);
	auto fragStr = ReadFile(fragmentShader);

	vertexCode = new char[vertexStr.size() + 1];
#if WIN32
	strcpy_s(vertexCode, vertexStr.size() + 1, vertexStr.c_str());
#else
	std::strcpy(vertexCode, vertexStr.c_str());
#endif

	fragmentCode = new char[fragStr.size() + 1];
#if WIN32
	strcpy_s(fragmentCode, fragStr.size() + 1, fragStr.c_str());
#else
	std::strcpy(fragmentCode, fragStr.c_str());
#endif

	CompileProgram();

	delete[] vertexCode;
	delete[] fragmentCode;
}

void Shader::DestroyShader()
{
	if (programId != 0) glDeleteProgram(programId);
	programId = 0;
}

void Shader::ReloadShader()
{
	if (programId == 0) return;
	DestroyShader();
	LoadShader(vertexPath.c_str(), fragmentPath.c_str());
}

void Shader::CompileProgram()
{
	programId = glCreateProgram();
	if (!programId)
		throw std::runtime_error("Canno't create GL program.");

	CompileShader(programId, vertexCode, GL_VERTEX_SHADER);
	CompileShader(programId, fragmentCode, GL_FRAGMENT_SHADER);

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

void Shader::Set(const char *name, const int value) const { glUniform1i(glGetUniformLocation(programId, name), value); }

void Shader::Set(const char *name, const bool value) const { glUniform1i(glGetUniformLocation(programId, name), static_cast<int>(value)); }

void Shader::Set(const char *name, const float value) const { glUniform1f(glGetUniformLocation(programId, name), value); }

void Shader::Set(const GLint id, const int value) const { glUniform1i(id, value); }

void Shader::Set(const GLint id, const bool value) const { glUniform1i(id, static_cast<int>(value)); }

void Shader::Set(const GLint id, const float value) const { glUniform1f(id, value); }

GLint Shader::GetUniform(const char *name) const { return glGetUniformLocation(programId, name); }
