#version 330 core

layout (location = 0) in vec3 InPosition;
layout (location = 1) in vec3 InNormal;
layout (location = 2) in vec3 InColor;
layout (location = 3) in vec2 InUV;

uniform mat4 ModelViewProjection;
uniform mat4 NormalMatrix;

out vec3 VertexColor;
out vec2 UV;
out vec3 Normal;


void main(){
	Normal = vec3(NormalMatrix * vec4(InNormal,0.0f));
	VertexColor = InColor;
	UV = InUV;
	gl_Position = ModelViewProjection * vec4(InPosition,1.0);
}