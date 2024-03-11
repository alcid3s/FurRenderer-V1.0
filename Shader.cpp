#include "Shader.h"

#include <iostream>
#include <fstream>

#include <glm/gtc/type_ptr.hpp>

bool checkShaderErrors(GLuint shaderId)
{
	GLint status;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &status);					//kijk of het compileren is gelukt
	if (status == GL_FALSE)
	{
		int length, charsWritten;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &length);				//haal de lengte van de foutmelding op
		char* infolog = new char[length + 1];
		memset(infolog, 0, length + 1);
		glGetShaderInfoLog(shaderId, length, &charsWritten, infolog);		//en haal de foutmelding zelf op
		std::cout << "Error compiling shader:\n" << infolog << std::endl;
		delete[] infolog;
		return true;
	}
	return false;
}



Shader::Shader(const std::string& filename)
{
	this->name = filename;
	reloadShaders();
}


void Shader::reloadShaders()
{
	if (programId != -1)
		glDeleteProgram(programId);

	uniforms.clear();

	std::ifstream vertexShaderFile(name + ".vs");
	std::string vertexShaderData((std::istreambuf_iterator<char>(vertexShaderFile)), std::istreambuf_iterator<char>());
	const char* cvertexShaderData = vertexShaderData.c_str();

	std::ifstream geometryShaderFile(name + ".gs");
	std::string geometryShaderData((std::istreambuf_iterator<char>(geometryShaderFile)), std::istreambuf_iterator<char>());
	const char* cgeometryShaderData = geometryShaderData.c_str();

	std::ifstream fragShaderFile(name + ".fs");
	std::string fragShaderData((std::istreambuf_iterator<char>(fragShaderFile)), std::istreambuf_iterator<char>());
	const char* cfragShaderData = fragShaderData.c_str();

	programId = glCreateProgram();							// maak een shaderprogramma aan

	if (vertexShaderFile) {
		GLuint vertexId = glCreateShader(GL_VERTEX_SHADER);		// maak vertex shader aan
		glShaderSource(vertexId, 1, &cvertexShaderData, NULL);		// laat opengl de shader uit de variabele 'vertexShader' halen
		glCompileShader(vertexId);								// compileer de shader
		if (checkShaderErrors(vertexId))
		{
			std::cout << "error in vs\n";
			glDeleteProgram(programId);
			programId = -1;
			return;
		}
		glAttachShader(programId, vertexId);					// hang de shader aan het shaderprogramma
	}

	if (geometryShaderFile) {
		GLuint geometryId = glCreateShader(GL_GEOMETRY_SHADER);		// maak vertex shader aan
		glShaderSource(geometryId, 1, &cgeometryShaderData, NULL);		// laat opengl de shader uit de variabele 'geometryShader' halen
		glCompileShader(geometryId);								// compileer de shader
		if (checkShaderErrors(geometryId))
		{
			std::cout << "error in gs\n";
			glDeleteProgram(programId);
			programId = -1;
			return;
		}
		glAttachShader(programId, geometryId);					// hang de shader aan het shaderprogramma
	}
	
	if (fragShaderFile) {
		GLuint fragmentId = glCreateShader(GL_FRAGMENT_SHADER);	// maak fragment shader aan
		glShaderSource(fragmentId, 1, &cfragShaderData, NULL);	// laat opengl de shader uit de variabele 'fragmentShader' halen
		glCompileShader(fragmentId);							// compileer de shader
		if (checkShaderErrors(fragmentId))
		{
			std::cout << "error in fs\n";
			glDeleteProgram(programId);
			programId = -1;
			return;
		}

		glAttachShader(programId, fragmentId);					// hang de shader aan het shaderprogramma
	}

	glBindAttribLocation(programId, 0, "a_position");		// zet de positie op vertex attribuut 0
	glBindAttribLocation(programId, 1, "a_color");			// zet de kleur op vertex attribuut 1
	glBindAttribLocation(programId, 2, "a_texcoord");		// zet de texcoord op vertex attribuut 2
	glBindAttribLocation(programId, 3, "a_normal");			// zet de normaal op vertex attribuut 3
	glLinkProgram(programId);								// link het programma, zorg dat alle attributes en varying gelinked zijn
	glUseProgram(programId);								// Zet dit als actieve programma
}

void Shader::use()
{
	glUseProgram(programId);								// Zet dit als actieve programma
}

GLuint Shader::getUniform(const std::string& name)
{
	auto it = uniforms.find(name);
	if (it != uniforms.end())
		return it->second;
	GLuint location = glGetUniformLocation(programId, name.c_str());
	uniforms[name] = location;
	return location;
}

void Shader::setUniform(const std::string& name, const glm::mat4& mat)
{
	glUniformMatrix4fv(getUniform(name), 1, GL_FALSE, glm::value_ptr(mat));
}

void Shader::setUniform(const std::string& name, int val)
{
	glUniform1i(getUniform(name), val);
}
