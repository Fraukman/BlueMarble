#version 330 core

uniform sampler2D TextureSampler;
uniform sampler2D TextureSpecularSampler;
uniform sampler2D TextureCloudsSampler;
uniform sampler2D TextureNightSampler;

uniform float Time;
uniform vec2 CloudsRotationSpeed;

uniform vec3 LightDirection;
uniform float LightIntensity;
uniform vec3 AmbientLight;

in vec3 VertexColor;
in vec2 UV;
in vec3 Normal;

out vec4 OutColor;

void main(){

	// ****** Textures Setup *******//
	vec3 DiffuseDayColor = texture(TextureSampler, UV).rgb;
	vec3 DiffuseNightColor = texture(TextureNightSampler, UV).rgb;

	vec3 TextureColor = texture(TextureSampler, UV).rgb;
	vec3 SpecularMap = texture(TextureSpecularSampler, UV).rgb;
	vec3 CloudsMap = texture(TextureCloudsSampler, UV + Time * CloudsRotationSpeed).rgb;

	vec3 N = normalize(Normal);
	vec3 L = -normalize(LightDirection);


	// ****** Ambient Light *******//
	float ka = 0.2;
	vec3 ia = AmbientLight;
	vec3 AmbientLight = ka * ia;

	// ****** Diffuse Light *******//
	float kd = 0.7;

	vec3 idDay = vec3(0.5f);
	idDay *= DiffuseDayColor + CloudsMap;

	vec3 idNight = vec3(0.50f);
	idNight *= DiffuseNightColor + CloudsMap * 0.3;
	
	float Lambertian = dot(N, L);

	vec3 CicleColor = mix(idNight, idDay, (Lambertian + 1.0) / 2.0);

	vec3 DiffuseLight = kd * CicleColor;


	// ****** Specular Light *******//
	float ks = SpecularMap.r;
	vec3 is = vec3(1.0f);
	vec3 ViewDirection = vec3(0, 0, -1);
	vec3 V = -ViewDirection;
	vec3 R = reflect(-L, N);
	float Alpha = 50.0;
	float Specular = pow(max(dot(R, V),0.0), Alpha);
	vec3 SpecularLight = ks * Specular * is;


	vec3 FinalColor = AmbientLight + (DiffuseLight * LightIntensity) + SpecularLight;
	OutColor = vec4(FinalColor,1.0);
}
