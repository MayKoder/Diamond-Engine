#include "Globals.h"
#include "M_Scene.h"

M_Scene::M_Scene(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

M_Scene::~M_Scene()
{
}

bool M_Scene::Init()
{
	return true;
}

bool M_Scene::Start()
{
	return true;
}

bool M_Scene::CleanUp()
{
	return true;
}
