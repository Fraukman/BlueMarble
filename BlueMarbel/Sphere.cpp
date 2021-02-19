#include "Sphere.h"


Sphere::Sphere(): VAO(0),VBO(0),EBO(0),NumOfIndices(0), NumOfVertices(0){}

void Sphere::GenerateSphereMesh(GLuint Resolution, std::vector<Vertex>& Vertices, std::vector<glm::ivec3>& Indices) {
	Vertices.clear();
	Indices.clear();

	constexpr float Pi = glm::pi<float>();
	constexpr float TwoPi = glm::two_pi<float>();
	float InvResolution = 1.0f / static_cast<float>(Resolution - 1);

	for (GLuint Uindex = 0; Uindex < Resolution; ++Uindex)
	{
		const float U = Uindex * InvResolution;
		const float Phi = glm::mix(0.0f, TwoPi, U);

		for (GLuint Vindex = 0; Vindex < Resolution; ++Vindex)
		{
			const float V = Vindex * InvResolution;
			const float Theta = glm::mix(0.0f, Pi, V);


			glm::vec3 VertexPosition{
				glm::sin(Theta) * glm::cos(Phi),
				glm::sin(Theta) * glm::sin(Phi),
				glm::cos(Theta)
			};

			Vertex vertex{
				VertexPosition,
				glm::normalize(VertexPosition),
				glm::vec3{1,1,1},
				glm::vec2(1 - U,V)
			};

			Vertices.push_back(vertex);
		}
	}

	for (GLuint U = 0; U < Resolution - 1; ++U)
	{
		for (GLuint V = 0; V < Resolution - 1; ++V)
		{
			GLuint P0 = U + V * Resolution;
			GLuint P1 = (U + 1) + V * Resolution;
			GLuint P2 = (U + 1) + (V + 1) * Resolution;
			GLuint P3 = U + (V + 1) * Resolution;

			Indices.push_back(glm::ivec3{ P0,P1,P3 });
			Indices.push_back(glm::ivec3{ P3,P1,P2 });
		}
	}


}

void Sphere::LoadSphere() {
	std::vector<Vertex> Vertices;
	std::vector<glm::ivec3> Triangles;
	GenerateSphereMesh(100, Vertices, Triangles);

	NumOfVertices = Vertices.size();
	NumOfIndices = Triangles.size() * 3;


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex), Vertices.data(), GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumOfIndices * sizeof(GLuint), Triangles.data(), GL_STATIC_DRAW);

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

	//vertices coordinates
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), nullptr);

	//Normal coordinates
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Normal)));

	//vertices color
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, Color)));

	//UV coordinates
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_TRUE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, UV)));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);

}

void Sphere::Draw()
{
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, NumOfIndices, GL_UNSIGNED_INT, nullptr);
	glBindVertexArray(0);
}
