#include "COMM_DeleteGO.h"

#include "Application.h"
#include "MO_Scene.h"

COMM_DeleteGO::COMM_DeleteGO(GameObject* agent) : Command(agent->UID),
	copy(*agent),
	copyParentUid(-1)
{
	if (agent->parent != nullptr)
		copyParentUid = agent->parent->UID;
}


COMM_DeleteGO::~COMM_DeleteGO()
{
}



void COMM_DeleteGO::Execute()
{
	EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, copy.UID)->Destroy();
}


void COMM_DeleteGO::Undo()
{
	if (copyParentUid != -1)
		EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, copyParentUid)->children.push_back(new GameObject(copy));

	else
		EngineExternal->moduleScene->root->children.push_back(new GameObject(copy));
}