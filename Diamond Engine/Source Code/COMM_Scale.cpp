#include "COMM_Scale.h"

#include "GameObject.h"
#include "CO_Transform.h"


COMM_Scale::COMM_Scale(GameObject* agent, float* sclChange) : Command(agent)
{
	scaleChange[0] = sclChange[0];
	scaleChange[1] = sclChange[1];
	scaleChange[2] = sclChange[2];
}


COMM_Scale::~COMM_Scale()
{
}


void COMM_Scale::Execute()
{
	agent->transform->localScale.x += scaleChange[0];
	agent->transform->localScale.y += scaleChange[1];
	agent->transform->localScale.z += scaleChange[2];

	agent->transform->updateTransform = true;
}


void COMM_Scale::Undo()
{
	agent->transform->localScale.x -= scaleChange[0];
	agent->transform->localScale.y -= scaleChange[1];
	agent->transform->localScale.z -= scaleChange[2];

	agent->transform->updateTransform = true;
}