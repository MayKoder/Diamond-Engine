#pragma once

#include"DEResource.h"
#include<vector>
#include"MathGeoLib/include/Math/float4x4.h"
#include"MathGeoLib/include/Math/float3.h"

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
	~ShaderVariable();

	GLuint vIndex;
	GLenum vType;
	GLint vSize;

	GLsizei nameLength;
	char name[25];

	union ShdrValue {
		ShdrValue();
		int intValue;
		float floatValue;
		GLuint textureValue;
		float4x4* matrixValue;
		float3 vector3Value;
	} data;
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