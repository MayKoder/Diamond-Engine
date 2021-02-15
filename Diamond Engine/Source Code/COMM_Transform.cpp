#include "COMM_Transform.h"

#include "GameObject.h"
#include "CO_Transform.h"


COMM_Transform::COMM_Transform(GameObject* agent, float* posChange, float* rotChange, float* sclChange) : Command(agent)
{
	positionChange[0] = posChange[0];
	positionChange[1] = posChange[1];
	positionChange[2] = posChange[2];

	rotationChange[0] = rotChange[0];
	rotationChange[1] = rotChange[1];
	rotationChange[2] = rotChange[2];

	scaleChange[0] = sclChange[0];
	scaleChange[1] = sclChange[1];
	scaleChange[2] = sclChange[2];
}


COMM_Transform::~COMM_Transform()
{
}


void COMM_Transform::Execute()
{
	agent->transform->position.x += positionChange[0];
	agent->transform->position.y += positionChange[1];
	agent->transform->position.z += positionChange[2];

	agent->transform->eulerRotation.x += rotationChange[0];
	agent->transform->eulerRotation.y += rotationChange[1];
	agent->transform->eulerRotation.z += rotationChange[2];

	agent->transform->localScale.x += scaleChange[0];
	agent->transform->localScale.y += scaleChange[1];
	agent->transform->localScale.z += scaleChange[2];

	agent->transform->updateTransform = true;
}


void COMM_Transform::Undo()
{
	agent->transform->position.x -= positionChange[0];
	agent->transform->position.y -= positionChange[1];
	agent->transform->position.z -= positionChange[2];

	agent->transform->eulerRotation.x -= rotationChange[0];
	agent->transform->eulerRotation.y -= rotationChange[1];
	agent->transform->eulerRotation.z -= rotationChange[2];

	agent->transform->localScale.x -= scaleChange[0];
	agent->transform->localScale.y -= scaleChange[1];
	agent->transform->localScale.z -= scaleChange[2];

	agent->transform->updateTransform = true;
}