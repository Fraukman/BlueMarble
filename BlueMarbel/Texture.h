#pragma once

#include <iostream>
#include <GL/glew.h>
#include <stb_image.h>
#include <vector>
#include <string>

class Texture 
{
public:
	Texture(const char* textureFile);
	Texture(std::vector<std::string> faces);

public:
	GLuint ID;
};

