#pragma once

#include"DEResource.h"
#include<vector>
typedef unsigned int GLuint;
class ResourceMaterial;

enum class ShaderType
{
	SH_Vertex,
	SH_Frag,
	SH_Max
};

class ResourceShader : public Resource
{

public:
	ResourceShader(unsigned int _uid);
	~ResourceShader();

	void LinkToProgram();

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void Bind();
	void Unbind();

	char* SaveShaderCustomFormat(char* vertexObjectBuffer, int vofSize, char* fragObjectBuffer, int fobSize);
	void LoadShaderCustomFormat(const char*);

	GLuint shaderObjects[static_cast<int>(ShaderType::SH_Max)];

	GLuint shaderProgramID;

	std::vector<ResourceMaterial*> references;
};