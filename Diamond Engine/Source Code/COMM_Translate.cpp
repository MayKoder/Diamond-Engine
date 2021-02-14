#include "COMM_Translate.h"

#include "GameObject.h"ç
#include "CO_Transform.h"


COMM_Translate::COMM_Translate(GameObject* agent, float* posChange) : Command(agent)
{
	positionChange[0] = posChange[0];
	positionChange[1] = posChange[1];
	positionChange[2] = posChange[2];
}


COMM_Translate::~COMM_Translate()
{
}


void COMM_Translate::Execute()
{
	agent->transform->position.x += positionChange[0];
	agent->transform->position.y += positionChange[1];
	agent->transform->position.z += positionChange[2];

	agent->transform->updateTransform = true;
}


void COMM_Translate::Undo()
{
	agent->transform->position.x -= positionChange[0];
	agent->transform->position.y -= positionChange[1];
	agent->transform->position.z -= positionChange[2];

	agent->transform->updateTransform = true;
}