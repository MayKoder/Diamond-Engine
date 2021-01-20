#pragma once

#include"DEResource.h"
#include<vector>

typedef unsigned int GLuint;
typedef unsigned int GLenum;
typedef int GLint;
typedef int GLsizei;

enum ShaderType
{
	SH_Vertex,
	SH_Frag,
	SH_Max
};

struct ShaderVariable
{
	ShaderVariable();

	GLuint vIndex;
	GLenum vType;
	GLint vSize;

	GLsizei nameLength;
	char name[25];
	void* data;
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

#ifndef STANDALONE
	void DrawEditor();
#endif // !STANDALONE


	void FillVariables();
	char* SaveShaderCustomFormat(char* vertexObjectBuffer, int vofSize, char* fragObjectBuffer, int fobSize);
	void LoadShaderCustomFormat(const char*);

	GLuint shaderObjects[SH_Max];

	GLuint shaderProgramID;

	std::vector<ShaderVariable> attributes;
	std::vector<ShaderVariable> uniforms;
};