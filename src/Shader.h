//
// Created by kaguya on 6/17/24.
//

#ifndef SHADERPLAYGROUND_SHADER_H
#define SHADERPLAYGROUND_SHADER_H

#include <GL/glew.h>
#include <format>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader
{
  public:
	void LoadShader(const char *vertexShader, const char *fragmentShader);
	void DestroyShader();
	void ReloadShader();
	void Use() const;
	void Set(const char *name, int value) const;
	void Set(const char *name, bool value) const;
	void Set(const char *name, float value) const;
	void Set(GLint id, int value) const;
	void Set(GLint id, bool value) const;
	void Set(GLint id, float value) const;
	[[nodiscard]] GLint GetUniform(const char *name) const;

	template <unsigned int size>
	void Set(const char *name, const glm::vec<size, int> &vector);

	template <unsigned int size>
	void Set(const char *name, const glm::vec<size, float> &vector);

	template <unsigned int size_x, unsigned int size_y>
	void Set(const char *name, const glm::mat<size_x, size_y, float> &matrix);

	template <unsigned int size>
	void Set(GLint id, const glm::vec<size, int> &vector);

	template <unsigned int size>
	void Set(GLint id, const glm::vec<size, float> &vector);

	template <unsigned int size_x, unsigned int size_y>
	void Set(GLint id, const glm::mat<size_x, size_y, float> &matrix);

  private:
	GLuint programId = 0;
	char *vertexCode = nullptr;
	char *fragmentCode = nullptr;
	std::string vertexPath;
	std::string fragmentPath;
	bool firstLoad = false;

	void CompileProgram();
	static GLuint CompileShader(GLuint program, const char *shaderCode, GLenum type);
	static std::string ReadFile(const char *file);
};

template <unsigned int size>
void Shader::Set(const char *name, const glm::vec<size, int> &vector)
{
	switch (size)
	{
	case 2:
		glUniform2iv(glGetUniformLocation(programId, name), 1, glm::value_ptr(vector));
		break;
	case 3:
		glUniform3iv(glGetUniformLocation(programId, name), 1, glm::value_ptr(vector));
		break;
	case 4:
		glUniform4iv(glGetUniformLocation(programId, name), 1, glm::value_ptr(vector));
		break;
	default:;
	}
}

template <unsigned int size>
void Shader::Set(const char *name, const glm::vec<size, float> &vector)
{
	switch (size)
	{
	case 2:
		glUniform2fv(glGetUniformLocation(programId, name), 1, glm::value_ptr(vector));
		break;
	case 3:
		glUniform3fv(glGetUniformLocation(programId, name), 1, glm::value_ptr(vector));
		break;
	case 4:
		glUniform4fv(glGetUniformLocation(programId, name), 1, glm::value_ptr(vector));
		break;
	default:;
	}
}

template <unsigned int size_x, unsigned int size_y>
void Shader::Set(const char *name, const glm::mat<size_x, size_y, float> &matrix)
{
	switch (size_x)
	{
	case 2:
		switch (size_y)
		{
		case 2:
			glUniformMatrix2fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 3:
			glUniformMatrix2x3fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 4:
			glUniformMatrix2x4fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		default:;
		}
		break;
	case 3:
		switch (size_y)
		{
		case 2:
			glUniformMatrix3x2fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 3:
			glUniformMatrix3fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 4:
			glUniformMatrix3x4fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		default:;
		}
		break;
	case 4:
		switch (size_y)
		{
		case 2:
			glUniformMatrix4x2fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 3:
			glUniformMatrix4x3fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 4:
			glUniformMatrix4fv(glGetUniformLocation(programId, name), 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		default:;
		}
	default:;
	}
}

template <unsigned int size>
void Shader::Set(const GLint id, const glm::vec<size, int> &vector)
{
	switch (size)
	{
	case 2:
		glUniform2iv(id, 1, glm::value_ptr(vector));
		break;
	case 3:
		glUniform3iv(id, 1, glm::value_ptr(vector));
		break;
	case 4:
		glUniform4iv(id, 1, glm::value_ptr(vector));
		break;
	default:;
	}
}

template <unsigned int size>
void Shader::Set(const GLint id, const glm::vec<size, float> &vector)
{
	switch (size)
	{
	case 2:
		glUniform2fv(id, 1, glm::value_ptr(vector));
		break;
	case 3:
		glUniform3fv(id, 1, glm::value_ptr(vector));
		break;
	case 4:
		glUniform4fv(id, 1, glm::value_ptr(vector));
		break;
	default:;
	}
}

template <unsigned int size_x, unsigned int size_y>
void Shader::Set(const GLint id, const glm::mat<size_x, size_y, float> &matrix)
{
	switch (size_x)
	{
	case 2:
		switch (size_y)
		{
		case 2:
			glUniformMatrix2fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 3:
			glUniformMatrix2x3fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 4:
			glUniformMatrix2x4fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		default:;
		}
		break;
	case 3:
		switch (size_y)
		{
		case 2:
			glUniformMatrix3x2fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 3:
			glUniformMatrix3fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 4:
			glUniformMatrix3x4fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		default:;
		}
		break;
	case 4:
		switch (size_y)
		{
		case 2:
			glUniformMatrix4x2fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 3:
			glUniformMatrix4x3fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		case 4:
			glUniformMatrix4fv(id, 1, GL_FALSE, glm::value_ptr(matrix));
			break;
		default:;
		}
	default:;
	}
}
#endif // SHADERPLAYGROUND_SHADER_H
