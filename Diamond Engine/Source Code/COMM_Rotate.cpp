#include "COMM_Rotate.h"

#include "GameObject.h"
#include "CO_Transform.h"


COMM_Rotate::COMM_Rotate(GameObject* agent, float* rotChange) : Command(agent)
{
	rotationChange[0] = rotChange[0];
	rotationChange[1] = rotChange[1];
	rotationChange[2] = rotChange[2];
}


COMM_Rotate::~COMM_Rotate()
{
}


void COMM_Rotate::Execute()
{
	agent->transform->eulerRotation.x += rotationChange[0];
	agent->transform->eulerRotation.y += rotationChange[1];
	agent->transform->eulerRotation.z += rotationChange[2];

	agent->transform->updateTransform = true;
}


void COMM_Rotate::Undo()
{
	agent->transform->eulerRotation.x -= rotationChange[0];
	agent->transform->eulerRotation.y -= rotationChange[1];
	agent->transform->eulerRotation.z -= rotationChange[2];

	agent->transform->updateTransform = true;
}