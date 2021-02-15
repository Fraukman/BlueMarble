#include "Shader.h"


Shader::Shader(const char* VertexShaderFile, const char* FragmentShaderFile) 
{
	std::string VertexShaderSource = ReadFile(VertexShaderFile);
	std::string FragmentShaderSource = ReadFile(FragmentShaderFile);

	GLuint VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	const char* VertexShaderSourcePtr = VertexShaderSource.c_str();
	glShaderSource(VertexShaderId, 1, &VertexShaderSourcePtr, nullptr);
	glCompileShader(VertexShaderId);
	CheckShader(VertexShaderId, GL_VERTEX_SHADER);

	const char* FragmentShaderSourcePtr = FragmentShaderSource.c_str();
	glShaderSource(FragmentShaderId, 1, &FragmentShaderSourcePtr, nullptr);
	glCompileShader(FragmentShaderId);
	CheckShader(FragmentShaderId, GL_FRAGMENT_SHADER);

	ID = glCreateProgram();
	glAttachShader(ID, VertexShaderId);
	glAttachShader(ID, FragmentShaderId);
	glLinkProgram(ID);

	GLint Result = GL_TRUE;
	glGetProgramiv(ID, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE) {
		GLint InfoLogLength;
		glGetProgramiv(ID, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0)
		{
			std::string InfoLog(InfoLogLength, '\0');
			glGetProgramInfoLog(ID, InfoLogLength, nullptr, &InfoLog[0]);
			std::cout << "Error Linking the program" << std::endl;
			std::cout << InfoLog << std::endl;
		}

		std::cerr << "Error Linking the program" << std::endl;
	}

	glDetachShader(ID, VertexShaderId);
	glDetachShader(ID, FragmentShaderId);

	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

}

std::string Shader::ReadFile(const char* FilePath) 
{
	std::string FileContents;
	if (std::ifstream FileStream{ FilePath,std::ios::in }) {
		FileContents.assign(std::istreambuf_iterator<char>(FileStream), std::istreambuf_iterator<char>());
	}
	else {
		std::cerr << "Error reading the file" << std::endl;
	}
	return FileContents;
}


void Shader::CheckShader(GLuint ShaderID, GLenum ShaderType)
{
	GLint Result = GL_TRUE;
	glGetShaderiv(ShaderID, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE) {
		GLint InfoLogLenght = 0;
		glGetShaderiv(ShaderID, GL_INFO_LOG_LENGTH, &InfoLogLenght);

		if (InfoLogLenght > 0) {
			std::string ShaderInfoLog(InfoLogLenght, '\0');
			glGetShaderInfoLog(ShaderID, InfoLogLenght, nullptr, &ShaderInfoLog[0]);
			std::string typeShader = (ShaderID == GL_VERTEX_SHADER) ? "VertexShader" : "FragmentShader";
			std::cout << "Error Compiling the " << typeShader << std::endl;
			std::cout << ShaderInfoLog << std::endl;
		}
	}
}