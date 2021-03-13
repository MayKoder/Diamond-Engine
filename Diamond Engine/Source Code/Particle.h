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
	float3 accel;
	float4 color;//RGBA

	float rotation;//DEGREES OR RAD? TODO CHECK
	float rotationSpeed;
	float maxLifetime;
	float currentLifetime;//also acts as isAlive bool
	float size;
};