#pragma once
#include "Module.h"
#include "Globals.h"
#include "glmath.h"
#include"Primitive.h"
#include "Light.h"

#include "OpenGL.h"

#define MAX_LIGHTS 8

class Mesh 
{

public:
	Mesh();
	~Mesh();

	void SetAsCube();
	void SetAsPyramid();
	void SetAsSphere();
	void SetAsCylinder();

	void RenderMesh();

private:

	//TODO: Vectors are really bad for this, maybe use float* or static array[]
	uint vertices_id;
	std::vector<float> vertices;

	uint indices_id;
	std::vector<float> indices;
};


class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	~ModuleRenderer3D();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnResize(int width, int height);
	void OnGUI() override;

	void GenerateFrameBuffer();

private:

	void GetCAPS(std::string& caps);
	std::string str_CAPS;

	bool vsync;
	bool wireframe;

	bool /*depth,*/ cull, lightng, color_material, texture_2d;

	uint my_id;

	Mesh testMesh;

public:
	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};