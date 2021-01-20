#pragma once

#include"DEResource.h"

class ResourceShader;
class C_Camera;

class DE_Cubemap
{

public:
	DE_Cubemap();
	~DE_Cubemap();

	//LoadCubeMap();
	void CreateGLData();
	void ClearMemory();

	void DrawAsSkybox(C_Camera* _transform);

	ResourceShader* shaderRes;
	unsigned int textureID;

	unsigned int vboId;
};

static const float skyboxVertices[] = 
{
	-500,  500, -500,
	-500, -500, -500,
	500, -500, -500,
	 500, -500, -500,
	 500,  500, -500,
	-500,  500, -500,

	-500, -500,  500,
	-500, -500, -500,
	-500,  500, -500,
	-500,  500, -500,
	-500,  500,  500,
	-500, -500,  500,

	 500, -500, -500,
	 500, -500,  500,
	 500,  500,  500,
	 500,  500,  500,
	 500,  500, -500,
	 500, -500, -500,

	-500, -500,  500,
	-500,  500,  500,
	 500,  500,  500,
	 500,  500,  500,
	 500, -500,  500,
	-500, -500,  500,

	-500,  500, -500,
	 500,  500, -500,
	 500,  500,  500,
	 500,  500,  500,
	-500,  500,  500,
	-500,  500, -500,

	-500, -500, -500,
	-500, -500,  500,
	 500, -500, -500,
	 500, -500, -500,
	-500, -500,  500,
	 500, -500,  500
};