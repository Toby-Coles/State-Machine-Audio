#pragma once
#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;


struct Light {
	XMFLOAT4 Ambient;
	XMFLOAT4 Diffuse;
	XMFLOAT4 Specular;
};

struct PointLight : Light
{
	PointLight() { ZeroMemory(this, sizeof(this)); }


	//Packed into a 4D Vector
	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Attenuation;
	float Pad;


};
struct SpotLight : Light
{
	//SpotLight() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT3 Position;
	float Range;

	XMFLOAT3 Direction;
	float Spot;

	XMFLOAT3 Attenuation;
	float pad;


};

struct Material : Light {
	Material() { ZeroMemory(this, sizeof(this)); }

	XMFLOAT4 Reflect;
};