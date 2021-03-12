#pragma once
#include "MathGeoLib/include/Math/float2.h"
#include "MathGeoLib/include/Math/float3.h"
#include "MathGeoLib/include/Math/float4.h"
#include "MathGeoLib/include/Math/Quat.h"

struct Particle
{
	Particle();
	~Particle();
	float3 pos;
	float3 speed;
	float4 color;//RGBA
	Quat rotation;
	float rotationSpeed;
	float maxLifetime;
	float currentLifetime;//also acts as isAlive bool
};