#ifndef STANDALONE

#include "COMM_Transform2D.h"
#include "Application.h"
#include "MO_Scene.h"

#include "GameObject.h"
#include "CO_Transform2D.h"

#include "MathGeoLib/include/Math/float4x4.h"

COMM_Transform2D::COMM_Transform2D(int agentUid, float* next_param, float* previous_param) : Command(agentUid)
{
	memcpy(next_parameters, next_param, sizeof(float) * 5);
	memcpy(previous_parameters, previous_param, sizeof(float) * 5);
}

COMM_Transform2D::~COMM_Transform2D()
{
}

void COMM_Transform2D::Execute()
{

	GameObject* agent = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, agentUid);

	if (agent != nullptr) {
		C_Transform2D* trans = static_cast<C_Transform2D*>(agent->GetComponent(Component::TYPE::TRANSFORM_2D));
		trans->SetTransform(next_parameters[0], next_parameters[1], next_parameters[2], next_parameters[3], next_parameters[4]);
		trans->SetTrueUpdateTransform();
	}

	else
		LOG(LogType::L_ERROR, "Couldn't redo transform, game object not found");
}

void COMM_Transform2D::Undo()
{
	GameObject* agent = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, agentUid);

	if (agent != nullptr) {
		C_Transform2D* trans = static_cast<C_Transform2D*>(agent->GetComponent(Component::TYPE::TRANSFORM_2D));
		trans->SetTransform(previous_parameters[0], previous_parameters[1], previous_parameters[2], previous_parameters[3], previous_parameters[4]);
		trans->SetTrueUpdateTransform();
	}

	else
		LOG(LogType::L_ERROR, "Couldn't undo transform, game object not found");
}

#endif // !STANDALONE