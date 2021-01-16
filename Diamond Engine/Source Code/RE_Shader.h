#pragma once

#include"DEResource.h"

typedef unsigned int GLuint;

enum Shader_OBType
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

	bool LoadToMemory() override;
	bool UnloadFromMemory() override;

	void SaveShaderCustomFormat();
	void LoadShaderCustomFormat();

	GLuint shaderObjects[SH_Max];

	GLuint shaderProgramID;
};