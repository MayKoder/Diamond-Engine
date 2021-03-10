#pragma once
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4.h"

struct Particle
{
	Particle();
	~Particle();
	float3 pos;
	float3 speed;
	float4 color;//RGBA
	float rotation;
	float rotationSpeed;
	float maxLifetime;
	float currentLifetime;//also acts as isAlive bool
};