#pragma once

#include <vector>

class Mesh;
class aiNode;
class aiMesh;
class GameObject;
typedef unsigned int GLuint;

//Change this to file system
namespace MeshLoader
{
	void EnableDebugMode();
	void DisableDebugMode();

	void logCallback(const char* message, char* user);

	/*void ImportFBX(const char* full_path, std::vector<Mesh*>& _mesh, GameObject* gmRoot);*/
	void ImportFBXFromBuffer(const char* full_path, char* buffer, int size, std::vector<Mesh*>& _mesh, GameObject* gmRoot);

	void NodeToGameObject(aiMesh** meshArray, std::vector<GLuint>& sceneTextures, std::vector<Mesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName);
	Mesh* LoadMesh(aiMesh* importedMesh, std::vector<GLuint>& materialsVector);

	void PopulateTransform(GameObject* child, aiNode* node);

	GLuint CustomLoadImage(char* buffer, int size, int* w = nullptr, int* h = nullptr);

}
