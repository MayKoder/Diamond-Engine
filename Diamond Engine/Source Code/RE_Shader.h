#pragma once

#include"DEResource.h"

typedef unsigned int GLuint;

enum ShaderType
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

	char* SaveShaderCustomFormat(char* vertexObjectBuffer, int vofSize, char* fragObjectBuffer, int fobSize);
	void LoadShaderCustomFormat(const char*);

	GLuint shaderObjects[SH_Max];

	GLuint shaderProgramID;
};