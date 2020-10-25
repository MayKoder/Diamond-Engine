#pragma once

#include "ModuleRenderer3D.h"
#include <vector>

class Mesh;
class aiScene;
class aiNode;
class aiMesh;
class GameObject;

namespace MeshLoader
{

	void EnableDebugMode();
	void DisableDebugMode();

	void logCallback(const char* message, char* user);

	void ImportFBX(const char* full_path, std::vector<Mesh*>& _mesh, GameObject* gmRoot);

	void NodeToGameObject(std::vector<Mesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName);
	Mesh* LoadMesh(aiMesh* importedMesh);

}

namespace StringLogic {

	std::string FileNameFromPath(const char* _path);

}