#pragma once

#include <vector>
#include"MathGeoLib/include/Math/Quat.h"

class ResourceMesh;
class aiNode;
class aiMesh;
class GameObject;
class ResourceTexture;
typedef unsigned int uint;

//Change this to file system
namespace MeshLoader
{
#ifndef STANDALONE
	void EnableDebugMode();
	void DisableDebugMode();
	void logCallback(const char* message, char* user);
#endif // !STANDALONE



	void NodeToGameObject(aiMesh** meshArray, std::vector<ResourceTexture*>& sceneTextures, std::vector<ResourceMesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName);
	ResourceMesh* LoadMesh(aiMesh* importedMesh, uint oldUID = 0);

	void PopulateTransform(GameObject* child, float3 position, Quat rotationQuat, float3 size);
}
