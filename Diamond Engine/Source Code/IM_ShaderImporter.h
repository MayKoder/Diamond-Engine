#pragma once

#include"RE_Shader.h"
#include<string>
typedef int GLint;

namespace ShaderImporter
{
	void Import(char* buffer, int bSize, ResourceShader* res, const char* assetsPath);

	int GetTypeMacro(ShaderType type);
	ShaderType GetAssetsObjType(const char* assetsPath, std::string& pairString);

	GLuint Compile(char* fileBuffer, ShaderType type, const GLint size);
}