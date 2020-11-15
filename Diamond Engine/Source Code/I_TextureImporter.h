#pragma once

typedef unsigned int GLuint;

namespace TextureImporter
{

	GLuint CustomLoadImage(char* buffer, int size, int* w = nullptr, int* h = nullptr);

	//Custom image system
	void SaveDDS(char* buffer, const char* fileName);
	char* LoadDDS(const char* fileName);

}