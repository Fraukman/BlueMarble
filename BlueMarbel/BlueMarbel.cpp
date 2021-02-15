#include <iostream>
#include <array>
#include <fstream>
#include <string>
#include <vector>

#include "GUI/imgui.h"
#include "GUI/imgui_impl_glfw.h"
#include "GUI/imgui_impl_opengl3.h"


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <glm/gtx/string_cast.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

unsigned int width = 800;
unsigned int height = 600;

std::string ReadFile(const char* FilePath)
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

void CheckShader(GLuint shaderId,GLenum shaderType) {
	GLint Result = GL_TRUE;
	glGetShaderiv(shaderId, GL_COMPILE_STATUS, &Result);

	if (Result == GL_FALSE) {
		GLint InfoLogLenght = 0;
		glGetShaderiv(shaderId, GL_INFO_LOG_LENGTH, &InfoLogLenght);

		if (InfoLogLenght > 0) {
			std::string ShaderInfoLog (InfoLogLenght,'\0');
			glGetShaderInfoLog(shaderId, InfoLogLenght, nullptr, &ShaderInfoLog[0]);
			std::string typeShader = (shaderType == GL_VERTEX_SHADER) ? "VertexShader" : "FragmentShader";
			std::cout << "Error Compiling the " <<typeShader<< std::endl;
			std::cout << ShaderInfoLog << std::endl;
		}
	}
}

GLuint LoadShaders(const char* VertexShaderFile, const char* FragmentShaderFile) 
{
	std::string VertexShaderSource = ReadFile(VertexShaderFile);
	std::string FragmentShaderSource = ReadFile(FragmentShaderFile);

	GLuint VertexShaderId = glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);

	const char* VertexShaderSourcePtr = VertexShaderSource.c_str();
	glShaderSource(VertexShaderId,1,&VertexShaderSourcePtr,nullptr);
	glCompileShader(VertexShaderId);
	CheckShader(VertexShaderId,GL_VERTEX_SHADER);

	const char* FragmentShaderSourcePtr = FragmentShaderSource.c_str();
	glShaderSource(FragmentShaderId, 1, &FragmentShaderSourcePtr, nullptr);
	glCompileShader(FragmentShaderId);
	CheckShader(FragmentShaderId,GL_FRAGMENT_SHADER);

	GLuint ProgramId = glCreateProgram();
	glAttachShader(ProgramId, VertexShaderId);
	glAttachShader(ProgramId, FragmentShaderId);
	glLinkProgram(ProgramId);

	GLint Result = GL_TRUE;
	glGetProgramiv(ProgramId, GL_LINK_STATUS, &Result);

	if (Result == GL_FALSE) {
		GLint InfoLogLength;
		glGetProgramiv(ProgramId, GL_INFO_LOG_LENGTH, &InfoLogLength);

		if (InfoLogLength > 0) 
		{
			std::string InfoLog(InfoLogLength, '\0');
			glGetProgramInfoLog(ProgramId, InfoLogLength, nullptr, &InfoLog[0]);
			std::cout<< "Error Linking the program" << std::endl;
			std::cout << InfoLog << std::endl;
		}

		std::cerr << "Error Linking the program" << std::endl;
	}

	glDetachShader(ProgramId,VertexShaderId);
	glDetachShader(ProgramId ,FragmentShaderId);

	glDeleteShader(VertexShaderId);
	glDeleteShader(FragmentShaderId);

	std::cout << "Deu Bom" << std::endl;

	return ProgramId;

}

GLuint LoadTexture(const char* TextureFile) 
{
	std::cout << "Loading texture " << TextureFile<<std::endl;

	stbi_set_flip_vertically_on_load(true);

	int TextureWidth = 0;
	int TextureHeight = 0;
	int NumberOfcomponents = 0;
	unsigned char* TextureData = stbi_load(TextureFile, &TextureWidth, &TextureHeight, &NumberOfcomponents, 3);

	if (!TextureData) {
		std::cout << "Error loading the texture " << TextureFile << std::endl;
		return 0;
	}

	GLuint TextureID;
	glGenTextures(1, &TextureID);
	glBindTexture(GL_TEXTURE_2D,TextureID);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, TextureWidth, TextureHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, TextureData);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glGenerateMipmap(GL_TEXTURE_2D);

	glBindTexture(GL_TEXTURE_2D, 0);
	stbi_image_free(TextureData);

	return TextureID;

}

std::vector<std::string> faces
{
		"CubeMap/right.jpg",
		 "CubeMap/left.jpg",
	   "CubeMap/bottom.jpg",
	      "CubeMap/top.jpg",
		"CubeMap/front.jpg",
		 "CubeMap/back.jpg"
};

GLuint LoadCubeMap(std::vector<std::string> faces) {
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		stbi_set_flip_vertically_on_load(true);
		unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;

}

struct Vertex {
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec3 Color;
	glm::vec2 UV;
};

struct DirectionalLight 
{
	glm::vec3 Diretion;
	GLfloat Intensity;
};

GLuint LoadGeometry() {
	std::array<Vertex, 6> Quad{
				//Position         Normal             Color          UV
		Vertex{glm::vec3{-1,-1,0},glm::vec3{0,0,1},glm::vec3{1,0,0},glm::vec2{0,0}},
		Vertex{glm::vec3(1,-1,0),glm::vec3{0,0,1},glm::vec3{0,1,0},glm::vec2{1,0}},
		Vertex{glm::vec3(1,1,0),glm::vec3{0,0,1},glm::vec3{1,0,0},glm::vec2{1,1}},
		Vertex{glm::vec3{-1,1,0},glm::vec3{0,0,1},glm::vec3{0,0,1},glm::vec2{0,1}},
	};

	std::array<glm::ivec3, 2> Indices{
		glm::vec3{0,1,3},
		glm::vec3{3,1,2}
	};

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Quad), Quad.data(), GL_STATIC_DRAW);

	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices.data(), GL_STATIC_DRAW);

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

	return VAO;


}

GLuint LoadCubeMapGeometry() {

	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	GLuint skyboxVAO, skyboxVBO;
	glGenVertexArrays(1, &skyboxVAO);
	glGenBuffers(1, &skyboxVBO);
	glBindVertexArray(skyboxVAO);
	glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

	
	return skyboxVAO;
}

void GenerateSphereMesh(GLuint Resolution, std::vector<Vertex>& Vertices, std::vector<glm::ivec3>& Indices)
{
	Vertices.clear();
	Indices.clear();
	
	constexpr float Pi = glm::pi<float>();
	constexpr float TwoPi = glm::two_pi<float>();
	float InvResolution =  1.0f / static_cast<float>(Resolution - 1);

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
				glm::vec2( 1 - U,V)
			};

			Vertices.push_back(vertex);
		}
	}

	for(GLuint U = 0; U < Resolution - 1; ++U)
	{
		for(GLuint V = 0; V < Resolution - 1; ++V)
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

GLuint LoadSphere(GLuint &NumVertices, GLuint &NumIndices) 
{
	std::vector<Vertex> Vertices;
	std::vector<glm::ivec3> Triangles;
	GenerateSphereMesh(100, Vertices,Triangles);

	NumVertices = Vertices.size();
	NumIndices = Triangles.size() * 3;

	
	
	GLuint VertexBuffer;
	glGenBuffers(1, &VertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
	glBufferData(GL_ARRAY_BUFFER, Vertices.size() * sizeof(Vertex),Vertices.data(),GL_STATIC_DRAW);
	
	GLuint EBO;
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, NumIndices * sizeof(GLuint),Triangles.data(),GL_STATIC_DRAW);

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
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


	return VAO;

}

class FlyCamera 
{
public:
	// View Matrix 
	glm::vec3 Location{ 0,0,5 };
	glm::vec3 Direction{ 0,0,-1 };
	glm::vec3 Up{ 0,1,0 };

	// Projection Matrix
	float FOV = glm::radians(45.0f);
	float NearPlane = 0.001;
	float FarPlane = 1000.0f;
	float AspectRatio = width / height;

	float speed = 3.0f;
	float MouseSensivity = 0.1f;

	glm::mat4 GetView() 
	{
		return glm::lookAt(Location, Location + Direction, Up);
	}

	glm::mat4 GetViewProjection() 
	{
		
		glm::mat4 ProjectionMatrix = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);

		return (ProjectionMatrix * GetView());
	}

	glm::mat4 GetProjection()
	{

		glm::mat4 ProjectionMatrix = glm::perspective(FOV, AspectRatio, NearPlane, FarPlane);

		return (ProjectionMatrix );
	}

	void Look(float Yaw, float Pitch) 
	{
		Yaw *= MouseSensivity;
		Pitch *= MouseSensivity;

		const glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));

		const glm::mat4 Id = glm::identity<glm::mat4>();
		glm::mat4 YawRotation = glm::rotate(Id, glm::radians(Yaw), Up);
		glm::mat4 PitchRotation = glm::rotate(Id, glm::radians(Pitch), Right);

		Up = PitchRotation * glm::vec4{ Up,0.0f };
		Direction = YawRotation * PitchRotation * glm::vec4{ Direction,0.0f };
	}

	void MoveFoward(float amount)
	{
		Location += glm::normalize(Direction) * amount * speed; 
	}

	void MoveRight(float amount)
	{
		glm::vec3 Right = glm::normalize(glm::cross(Direction, Up));
		Location += Right * amount * speed;
	}

};

//Camera Object
FlyCamera Camera;
bool bEnableMovement = false;
glm::vec2 PreviousCursor{ 0,0 };

bool DebugMode = true;


void MouseButtonCallback(GLFWwindow* Window, int Button, int Action, int Modifiers)
{
	if (Button == GLFW_MOUSE_BUTTON_LEFT) 
	{
		if (Action == GLFW_PRESS && DebugMode == false)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			double X, Y;
			glfwGetCursorPos(Window, &X, &Y);

			PreviousCursor = glm::vec2{ X,Y };
			bEnableMovement = true;
		}
		if (Action == GLFW_RELEASE && DebugMode == false)
		{
			glfwSetInputMode(Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

			bEnableMovement = false;
		}
	}
}

void MouseMotionCallback(GLFWwindow* Window, double X, double Y) 
{
	if (bEnableMovement)
	{
		glm::vec2 CurrentCursor{ X,Y };
		glm::vec2 DeltaCursor = CurrentCursor - PreviousCursor;

		//std::cout << glm::to_string(DeltaCursor) << std::endl;

		Camera.Look(-DeltaCursor.x, -DeltaCursor.y);

		PreviousCursor = CurrentCursor;
	}
}

void Resize(GLFWwindow * Window,int NewWidth, int NewHeight)
{
	width = NewWidth;
	height = NewHeight;

	Camera.AspectRatio = static_cast<float>(width) / height;
	glViewport(0, 0, width, height);
}


int main() {


	if (glfwInit() != GLFW_TRUE) {
		std::cerr << "Error initializing glfw" << std::endl;
		return -1;
	}

	GLFWwindow* window = glfwCreateWindow(width, height, "Blue Marbel", nullptr, nullptr);
	if (!window) {
		std::cerr << "Error creating window" << std::endl;
		return -1;
	}

	Resize(window,width,height);

	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, MouseMotionCallback);
	glfwSetFramebufferSizeCallback(window, Resize);

	glfwMakeContextCurrent(window);

	if (glewInit() != GLEW_OK) {
		std::cerr << "Error initializing GLEW" << std::endl;
		return -1;
	}

	GLint glewMinVersion = 0;
	GLint glewMaxVersion = 0;

	glGetIntegerv(GL_MAJOR_VERSION, &glewMaxVersion);
	glGetIntegerv(GL_MINOR_VERSION, &glewMinVersion);
	std::cout << "version: " << glewMaxVersion << "." << glewMinVersion << std::endl;
	std::cout << "DRIVER vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "GLSL version " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
	const char* glsl_version = "#version 130";

	// enable backface culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//enable Depth buffer
	glEnable(GL_DEPTH_TEST);
	//glDepthFunc(GL_LESS);

	GLuint ProgramId = LoadShaders("Shaders/triangle_vert.glsl", "Shaders/triangle_frag.glsl");
	GLuint SkyBoxShader = LoadShaders("Shaders/skybox_vert.glsl", "Shaders/skybox_frag.glsl");

	GLuint DiffuseDayID = LoadTexture("Textures/earth_2k.jpg");
	GLuint TextureSpecularID = LoadTexture("Textures/2k_earth_specular_map.jpg");
	GLuint TextureCloudsID = LoadTexture("Textures/2k_earth_clouds.jpg");
	GLuint DiffuseNightID = LoadTexture("Textures/2k_earth_nightmap.jpg");


	//cubeMap Geometry
	GLuint CubeMap = LoadCubeMapGeometry();

	// CubeMap texture
	GLuint CubeMapTexture = LoadCubeMap(faces);

	//GLuint QuadVAO = LoadGeometry();

	GLuint numofVertices = 0;
	GLuint numofIndices = 0;
	GLuint SphereGeometry = LoadSphere(numofVertices,numofIndices);

	std::cout << "Num of vertices: " << numofVertices << std::endl;
	std::cout << "Num of indices: " << numofIndices << std::endl;

	//Model matrix

	glm::mat4 I = glm::identity<glm::mat4>();
	glm::mat4 ModelMatrix = glm::rotate(I, glm::radians(90.0f), glm::vec3{ 1,0,0 });


	glClearColor(0.55, 0.55, 0.55, 1.0f);

	double PreviousTime = glfwGetTime();



	DirectionalLight Light;
	Light.Diretion = glm::vec3{ 0.0f,0.0f,-1.0f };
	Light.Intensity = 2.0f;
	float angleRot = 5;



	glUseProgram(ProgramId);

	glUseProgram(SkyBoxShader);
	GLint CubeMapSampler = glGetUniformLocation(SkyBoxShader, "skybox");
	glUniform1i(CubeMapSampler, 4);
	

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	ImVec4 lightDir = ImVec4(0.0f, 0.0f, -1.0f, 0.0f);
	ImVec4 AmbientColor = ImVec4(0.1f, 0.3f, 0.7f,1.0f);
	ImVec2 CloudSpeed( 0.01f, 0.0f );

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();


		double CurrentTime = glfwGetTime();
		double DeltaTime = CurrentTime - PreviousTime;
		if (DeltaTime > 0.01) { PreviousTime = CurrentTime; }
					
		ModelMatrix = glm::rotate(ModelMatrix, glm::radians(angleRot * (float)DeltaTime), glm::vec3{ 0,0,1 });
				
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(ProgramId);

		glm::mat4 NormalMatrix = glm::inverse(glm::transpose(Camera.GetView() * ModelMatrix));
		glm::mat4 ViewProjectionMatrix = Camera.GetViewProjection();
		glm::mat4 ModelViewProjection = ViewProjectionMatrix * ModelMatrix;


		GLint TimeID = glGetUniformLocation(ProgramId, "Time");
		glUniform1f(TimeID, CurrentTime);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, DiffuseDayID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, TextureSpecularID);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, TextureCloudsID);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, DiffuseNightID);

		GLint TextureDiffuseSamplerID = glGetUniformLocation(ProgramId, "TextureSampler");
		GLint TextureSpecularSampler = glGetUniformLocation(ProgramId, "TextureSpecularSampler");
		GLint TextureCloudsSampler = glGetUniformLocation(ProgramId, "TextureCloudsSampler");
		GLint TextureNightSampler = glGetUniformLocation(ProgramId, "TextureNightSampler");

		GLint ModelViewProjectionID = glGetUniformLocation(ProgramId, "ModelViewProjection");
		glUniformMatrix4fv(ModelViewProjectionID, 1, GL_FALSE, glm::value_ptr(ModelViewProjection));

		GLint NormalMatrixID = glGetUniformLocation(ProgramId, "NormalMatrix");
		glUniformMatrix4fv(NormalMatrixID,1,GL_FALSE,glm::value_ptr(NormalMatrix));

		GLint LightDirectionID = glGetUniformLocation(ProgramId, "LightDirection");
		glUniform3fv(LightDirectionID, 1, glm::value_ptr(Camera.GetView() * glm::vec4{ lightDir.x,lightDir.y,lightDir.z,0.0f }));

		GLint AmbientLightID = glGetUniformLocation(ProgramId, "AmbientLight");
		glUniform3fv(AmbientLightID, 1, glm::value_ptr(glm::vec3{ AmbientColor.x,AmbientColor.y,AmbientColor.z}));

		GLint CloudSpeedID = glGetUniformLocation(ProgramId, "CloudsRotationSpeed");
		glUniform2fv(CloudSpeedID, 1, glm::value_ptr(glm::vec2{ CloudSpeed.x,CloudSpeed.y }));

		GLint LightIntensityID = glGetUniformLocation(ProgramId, "LightIntensity");
		glUniform1f(LightIntensityID, Light.Intensity);

		
		glUniform1i(TextureDiffuseSamplerID, 0);
		glUniform1i(TextureSpecularSampler, 1);
		glUniform1i(TextureCloudsSampler, 2);
		glUniform1i(TextureNightSampler, 3);

		//glBindVertexArray(QuadVAO);
		glBindVertexArray(SphereGeometry);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINES);
		glDrawElements(GL_TRIANGLES, numofIndices, GL_UNSIGNED_INT, nullptr);
		
		glBindVertexArray(0);
				
		

		glUseProgram(SkyBoxShader);

		glm::mat4 viewMatrix = glm::mat4(glm::mat3(Camera.GetView()));
		glm::mat4 projection = Camera.GetProjection();

		GLint ViewID = glGetUniformLocation(SkyBoxShader, "View");
		GLint ProjectionID = glGetUniformLocation(SkyBoxShader, "Projection");
		glUniformMatrix4fv(ViewID, 1, GL_FALSE, glm::value_ptr(viewMatrix));
		glUniformMatrix4fv(ProjectionID, 1, GL_FALSE, glm::value_ptr(projection));

		glBindVertexArray(CubeMap);
		glActiveTexture(GL_TEXTURE4);
		glBindTexture(GL_TEXTURE_CUBE_MAP, CubeMapTexture);
		glDepthFunc(GL_LEQUAL);
		glDrawArrays(GL_TRIANGLES, 0, 36);
		glBindVertexArray(0);
		glDepthFunc(GL_LESS);

		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		static float f = 0.0f;
		float &f2 = angleRot;
		static int counter = 0;
		static int debugInt = (DebugMode) ? 1 : 0;


		ImGui::Begin("Blue Marble Project!");
		ImGui::Text("debug mode = %i", debugInt);

		ImGui::Text("Clouds Rotation Speed");
		ImGui::SliderFloat2("Speed", (float*)&CloudSpeed, 0.0f, 0.5f);;

		ImGui::Text("Earth Rotation Speed");
		ImGui::SliderFloat("Angles per Second", &angleRot, -30.0f, 30.0f);

		ImGui::SliderFloat3("Light Direction", (float*)&lightDir, -1.0f, 1.0f);
		ImGui::SliderFloat("Light Intensity", &Light.Intensity, 0.0f, 10.0f);
		ImGui::ColorEdit3("Ambient Color", (float*)&AmbientColor);

		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
		

		
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());



		glfwSwapBuffers(window);

		if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		{
			Camera.MoveFoward(1.0f * DeltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		{
			Camera.MoveFoward(-1.0f * DeltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		{
			Camera.MoveRight(1.0f * DeltaTime);
		}

		if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		{
			Camera.MoveRight(-1.0f * DeltaTime);
		}
		if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS)
		{
			DebugMode = !DebugMode;
		}

	}

	glDeleteVertexArrays(1, &SphereGeometry);
	glDeleteVertexArrays(1, &CubeMap);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}