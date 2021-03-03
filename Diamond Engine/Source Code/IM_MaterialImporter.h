#pragma once

class ResourceMaterial;

namespace MaterialImporter
{
	void CreateBaseMaterialFile(const char* path);
	void Save(ResourceMaterial* material, char**fileBuffer);
	//void Save(uint uid, const char* path);
}
