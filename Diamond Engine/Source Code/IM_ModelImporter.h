#pragma once
#include<vector>

class GameObject;
class Resource;
class ResourceMesh;
typedef unsigned int uint;

namespace ModelImporter
{
	void Import(char* buffer, int bSize, Resource* res);

	void SaveModelCustom(GameObject* root, const char* nameWithExtension);
	void LoadModelCustom(const char* nameWithExtension);

	void SaveMeshesToMeta(const char* assetFile, std::vector<ResourceMesh*>& meshes);
	void GetMeshesFromMeta(const char* assetFile, std::vector<uint>& uids);
}