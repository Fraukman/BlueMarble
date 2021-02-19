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

#include "Texture.h"
#include "Shader.h"
#include "Sphere.h"
#include "CubeMap.h"
#include "FlyCamera.h"

unsigned int width = 800;
unsigned int height = 600;


std::vector<std::string> faces
{
		"CubeMap/right.jpg",
		 "CubeMap/left.jpg",
	   "CubeMap/bottom.jpg",
	      "CubeMap/top.jpg",
		"CubeMap/front.jpg",
		 "CubeMap/back.jpg"
};

struct DirectionalLight 
{
	glm::vec3 Diretion;
	GLfloat Intensity;
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

	GLFWwindow* window = glfwCreateWindow(width, height, "Blue Marble", nullptr, nullptr);
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

	// enable back face culling
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	//enable Depth buffer
	glEnable(GL_DEPTH_TEST);



	Shader EarthShader("Shaders/triangle_vert.glsl", "Shaders/triangle_frag.glsl");
	Shader SkyBoxShader("Shaders/skybox_vert.glsl", "Shaders/skybox_frag.glsl");

	Texture DiffuseTexture("Textures/earth_2k.jpg");
	Texture SpecularTexture("Textures/2k_earth_specular_map.jpg");
	Texture CloudsTexture("Textures/2k_earth_clouds.jpg");
	Texture DiffuseNightTexture("Textures/2k_earth_nightmap.jpg");

	// CubeMap Geometry
	CubeMap cubeMap;
	cubeMap.LoadCubeMapGeometry();
	// CubeMap texture
	Texture CubeMapTextures(faces);

	Sphere SphereGeometry;
	SphereGeometry.LoadSphere();

	//Model matrix
	glm::mat4 I = glm::identity<glm::mat4>();
	glm::mat4 ModelMatrix = glm::rotate(I, glm::radians(90.0f), glm::vec3{ 1,0,0 });


	glClearColor(0.55, 0.55, 0.55, 1.0f);

	double PreviousTime = glfwGetTime();

	DirectionalLight Light;
	Light.Diretion = glm::vec3{ 0.0f,0.0f,-1.0f };
	Light.Intensity = 2.0f;
	float angleRot = 5;

	EarthShader.use();
	EarthShader.setInt("TextureSampler", 0);
	EarthShader.setInt("TextureSpecularSampler", 1);
	EarthShader.setInt("TextureCloudsSampler", 2);
	EarthShader.setInt("TextureNightSampler", 3);

	SkyBoxShader.use();
	SkyBoxShader.setInt("skybox", 4);


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
		
		EarthShader.use();

		glm::mat4 NormalMatrix = glm::inverse(glm::transpose(Camera.GetView() * ModelMatrix));
		glm::mat4 ViewProjectionMatrix = Camera.GetViewProjection();
		glm::mat4 ModelViewProjection = ViewProjectionMatrix * ModelMatrix;

		EarthShader.setFloat("Time", CurrentTime);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, DiffuseTexture.ID);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, SpecularTexture.ID);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, CloudsTexture.ID);

		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, DiffuseNightTexture.ID);

		EarthShader.setMat4("ModelViewProjection", ModelViewProjection);
		EarthShader.setMat4("NormalMatrix", NormalMatrix);
		EarthShader.setVec3("LightDirection", Camera.GetView() * glm::vec4{ lightDir.x,lightDir.y,lightDir.z,0.0f });
		EarthShader.setVec3("AmbientLight", glm::vec3{ AmbientColor.x,AmbientColor.y,AmbientColor.z });
		EarthShader.setVec2("CloudsRotationSpeed", glm::vec2{ CloudSpeed.x,CloudSpeed.y });
		EarthShader.setFloat("LightIntensity", Light.Intensity);

		SphereGeometry.Draw();

		SkyBoxShader.use();
		glm::mat4 viewMatrix = glm::mat4(glm::mat3(Camera.GetView()));
		glm::mat4 projectionMatrix = Camera.GetProjection();
		SkyBoxShader.setMat4("View", viewMatrix);
		SkyBoxShader.setMat4("Projection", projectionMatrix);

		cubeMap.Draw(CubeMapTextures);

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

	glDeleteVertexArrays(1, &SphereGeometry.VAO);
	glDeleteVertexArrays(1, &cubeMap.VAO);

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();

	return 0;
}