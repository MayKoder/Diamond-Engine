#pragma once

class GameObject;
class Resource;

namespace ModelImporter
{
	void Import(char* buffer, int bSize, Resource* res);

	void SaveModelCustom(GameObject* root, const char* nameWithExtension);
	void LoadModelCustom(const char* nameWithExtension);

}