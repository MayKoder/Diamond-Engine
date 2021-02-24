#pragma once

#include<vector>
#include<string>
typedef unsigned int GLuint;
class Resource;
class DE_Cubemap;

namespace TextureImporter
{
	void Init();

	GLuint LoadToMemory(char* buffer, int size, int* w = nullptr, int* h = nullptr);
	void SaveDDS(char* buffer, int size, const char* fileName);

	void Import(char* buffer, int bSize, Resource* res);

	void TakeScreenshot(int frameBuffer);

	void LoadCubeMap(std::vector<std::string>& faces, DE_Cubemap& cubeMap);
}