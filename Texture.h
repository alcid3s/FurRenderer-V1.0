#pragma once

#include <windows.h>
#include <glad/glad.h>
#include <string>

class Texture
{
public:
	Texture(std::string);
	~Texture();

	GLuint textureId;
};

