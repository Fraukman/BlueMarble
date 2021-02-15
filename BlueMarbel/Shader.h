#pragma once

#include <iostream>
#include <string>
#include <fstream>

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

class Shader {

private:
	std::string ReadFile(const char* FilePath);
	void CheckShader(GLuint ShaderID, GLenum ShaderType);

public:

	Shader(const char* VertexShaderFile, const char* FragmentShaderFile);
	
	void use() 
	{
		glUseProgram(ID);
	}

	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), static_cast<int> (value));
	}

	void setFloat(const std::string &name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), static_cast<float> (value));
	}

	void setVec2(const std::string &name, glm::vec2 value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1,glm::value_ptr(value));
	}

	void setVec3(const std::string &name, glm::vec3 value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1,glm::value_ptr(value));
	}

	void setMat4(const std::string &name, glm::mat4 value) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),1,GL_FALSE,glm::value_ptr(value));
	}

public:
	GLuint ID;



};
