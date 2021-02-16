#include "COMM_Transform.h"

#include "GameObject.h"
#include "CO_Transform.h"

#include "MathGeoLib/include/Math/float4x4.h"

COMM_Transform::COMM_Transform(GameObject* agent, float* nextMat, float* previousMat) : Command(agent)
{
	for (int i = 0; i < 16; i++)
	{
		nextMatrix[i] = nextMat[i];
		previousMatrix[i] = previousMat[i];
	}
}


COMM_Transform::~COMM_Transform()
{
}


void COMM_Transform::Execute()
{
	float4x4 mat;
	mat.Set(nextMatrix);
	agent->transform->SetTransformWithGlobal(mat);
}


void COMM_Transform::Undo()
{
	float4x4 mat;
	mat.Set(previousMatrix);
	agent->transform->SetTransformWithGlobal(mat);
}