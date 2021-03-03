#pragma once
#include<vector>

class GameObject;
class Resource;
class ResourceMesh;
class ResourceAnimation;
class aiScene;
typedef unsigned int uint;

namespace ModelImporter
{
	void Import(char* buffer, int bSize, Resource* res);

	void ImportAnimations(const aiScene* scene, std::vector<uint>& animationsUIDs, std::vector<ResourceAnimation*>& animationsOnModelUIDs, Resource* res);

	void SaveModelCustom(GameObject* root, const char* nameWithExtension);
	void LoadModelCustom(const char* nameWithExtension);

	void SaveMeshesToMeta(const char* assetFile, std::vector<ResourceMesh*>& meshes);
	void GetMeshesFromMeta(const char* assetFile, std::vector<uint>& uids);

	void SaveAnimationsToMeta(const char* assetFile, std::vector<ResourceAnimation*>& animation);
	void GetAnimationsFromMeta(const char* assetFile, std::vector<uint>& uids);
}