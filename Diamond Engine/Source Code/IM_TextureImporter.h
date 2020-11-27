#pragma once

typedef unsigned int GLuint;
class Resource;

namespace TextureImporter
{

	GLuint CustomLoadImage(char* buffer, int size, int* w = nullptr, int* h = nullptr);

	//Custom image system
	void SaveDDS(char* buffer, int size, const char* fileName);
	char* LoadDDS(const char* fileName);

	void Import(char* buffer, int bSize, Resource* res);

	void TakeScreenshot(int frameBuffer);

}