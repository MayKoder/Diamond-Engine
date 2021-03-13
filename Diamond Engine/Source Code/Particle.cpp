#include "Particle.h"
//#include "MathGeoLib/include/Math/float3.h"
//#include "MathGeoLib/include/Math/float4.h"

Particle::Particle(): pos(0.0f,0.0f,0.0f),speed(0.0f, 0.0f, 0.0f),color(1.0f,1.0f,1.0f,1.0f),
rotation(0.0f),rotationSpeed(0.0f),maxLifetime(-1.0f),currentLifetime(-1.0f),size(1.0f),accel(0.0f,0.0f,0.0f)
{
}

Particle::~Particle()
{
}
