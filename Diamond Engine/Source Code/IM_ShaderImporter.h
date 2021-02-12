#pragma once

#include"RE_Shader.h"
#include<string>
typedef int GLint;

struct TempShader {
	std::pair<size_t, char*> data;
	int tmpID;
};

namespace ShaderImporter
{
	void Import(char* buffer, int bSize, ResourceShader* res, const char* assetsPath);

	bool CheckForErrors(std::string& glslBuffer, TempShader& vertexShader, TempShader& fragmentShader);

	int GetTypeMacro(ShaderType type);
	ShaderType GetAssetsObjType(const char* assetsPath, std::string& pairString);

	void CreateBaseShaderFile(const char* path);

	GLuint Compile(char* fileBuffer, ShaderType type, const GLint size);
}