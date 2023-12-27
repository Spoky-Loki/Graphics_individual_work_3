#ifndef SHADER_H
#define SHADER_H

#include "Header.h"

void ShaderLog(unsigned int shader);
void checkOpenGLerror();

class shader
{
public:
    GLuint Program;
    shader(const char* VertexShaderSource, const char* FragShaderSource)
    {
		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vShader, 1, &VertexShaderSource, NULL);
		glCompileShader(vShader);
		std::cout << "vertex shader \n";
		ShaderLog(vShader);
		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fShader, 1, &FragShaderSource, NULL);
		glCompileShader(fShader);
		std::cout << "fragment shader \n";
		ShaderLog(fShader);
		Program = glCreateProgram();
		glAttachShader(Program, vShader);
		glAttachShader(Program, fShader);
		glLinkProgram(Program);
		int link_ok;
		glGetProgramiv(Program, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			std::cout << "error attach shaders \n";
			return;
		}
		checkOpenGLerror();
    }
	void Use()
	{
		glUseProgram(this->Program);
	}

	void ReleaseShader() {
		glUseProgram(0);
		glDeleteProgram(Program);
	}
};


void ShaderLog(unsigned int shader)
{
	int infologLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infologLen);
	if (infologLen > 1)
	{
		int charsWritten = 0;
		std::vector<char> infoLog(infologLen);
		glGetShaderInfoLog(shader, infologLen, &charsWritten, infoLog.data());
		std::cout << "InfoLog: " << infoLog.data() << std::endl;
	}
}

void checkOpenGLerror() {
	GLenum errCode;
	if ((errCode = glGetError()) != GL_NO_ERROR)
		std::cout << "OpenGl error!: " << errCode << std::endl;
}
#endif