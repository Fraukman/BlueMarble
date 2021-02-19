#pragma once
#include <GL/glew.h>

class FlyCamera
{
public:
	
	unsigned int width = 800;
	unsigned int height = 600;

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

		return (ProjectionMatrix);
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