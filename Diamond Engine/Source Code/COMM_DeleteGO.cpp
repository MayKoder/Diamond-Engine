#ifndef STANDALONE

#include "COMM_DeleteGO.h"

#include "Application.h"
#include "MO_Scene.h"

#include "GameObject.h"

COMM_DeleteGO::COMM_DeleteGO(GameObject* agent) : Command(agent->UID),
	value(nullptr),
	copyParentUid(-1),
	agentName(agent->name)
{
	value = json_value_init_object();
	DEConfig copy(json_value_get_object(value));

	JSON_Value* arrayGO = json_value_init_array();
	agent->SaveToJson(json_value_get_array(arrayGO));

	json_object_set_value(copy.nObj, "Game Objects", arrayGO);

	if (agent->parent != nullptr)
		copyParentUid = agent->parent->UID;
}


COMM_DeleteGO::~COMM_DeleteGO()
{
	json_value_free(value);
}


void COMM_DeleteGO::Execute()
{
	GameObject* agent = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, agentUid);

	if (agent != nullptr)
		agent->Destroy();

	else
		LOG(LogType::L_ERROR, "Couldn't redo delete, game object not found");
}


void COMM_DeleteGO::Undo()
{
	DEConfig copy(json_value_get_object(value));

	JSON_Array* arrayGO = copy.ReadArray("Game Objects");

	GameObject* parent = nullptr;
	
	if (copyParentUid != -1)
		parent = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, copyParentUid);

	else if (parent == nullptr)		//Just in case the parent cant be found
		parent = EngineExternal->moduleScene->root;

	for (size_t i = 0; i < json_array_get_count(arrayGO); i++)
	{
		parent = EngineExternal->moduleScene->LoadGOData(json_array_get_object(arrayGO, i), parent);
	}

	EngineExternal->moduleScene->LoadScriptsData();
}


#endif // !STANDALONE