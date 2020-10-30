#pragma once
#include "Application.h"
#include "Module.h"
//#include "Globals.h"
#include "glmath.h"
#include"Primitive.h"
#include "Light.h"

#include <queue>

#include "OpenGL.h"

class Mesh;
class C_MeshRenderer;
class Texture;

#define MAX_LIGHTS 1

#define SQUARE_TEXTURE_W 256
#define SQUARE_TEXTURE_H 256

class ModuleRenderer3D : public Module
{
public:
	ModuleRenderer3D(Application* app, bool start_enabled = true);
	virtual ~ModuleRenderer3D();

	bool Init() override;
	update_status PreUpdate(float dt) override;
	update_status PostUpdate(float dt) override;
	bool CleanUp() override;

	void OnResize(int width, int height);
	void OnGUI() override;

	void ReGenerateFrameBuffer(int w, int h);

private:

	void GetCAPS(std::string& caps);
	std::string str_CAPS;

public:
	bool vsync, wireframe, cull, lightng, color_material, texture_2d;

	unsigned int framebuffer;
	unsigned int texColorBuffer;
	unsigned int rbo;

	//Mesh testMesh;
	std::vector<Mesh*> globalMeshes;
	std::vector<GLuint> globalTextures;
	std::vector<Texture> Textures;

	GLuint checkersTexture;
	GLubyte checkerImage[SQUARE_TEXTURE_W][SQUARE_TEXTURE_H][4];

	std::vector<C_MeshRenderer*> renderQueue;

public:

	Light lights[MAX_LIGHTS];
	SDL_GLContext context;
	mat3x3 NormalMatrix;
	mat4x4 ModelMatrix, ViewMatrix, ProjectionMatrix;
};