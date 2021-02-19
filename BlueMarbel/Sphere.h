#pragma once
#include "Vertex.h"
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <vector>

class Sphere 
{
private:
	void GenerateSphereMesh(GLuint Resolution, std::vector<Vertex>& Vertices, std::vector<glm::ivec3>& Indices);

public:
	Sphere();
	void LoadSphere();

	void Draw();

	GLuint VAO;
	GLuint VBO;
	GLuint EBO;

	GLuint NumOfIndices;
	GLuint NumOfVertices;
};
