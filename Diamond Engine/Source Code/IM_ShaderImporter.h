#pragma once

class Resource;

namespace ShaderImporter
{
	void Import(char* buffer, int bSize, Resource* res);

	int GetAssetsShaderType(const char* path);
}