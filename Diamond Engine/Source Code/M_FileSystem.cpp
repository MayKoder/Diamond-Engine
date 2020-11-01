#include "M_FileSystem.h"
#include "Globals.h"

#include "MeshLoader.h"
#include "I_FileSystem.h"

#include "Application.h"

#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#include "DevIL\include\ilu.h"
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#include "DevIL\include\ilut.h"
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )


M_FileSystem::M_FileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

M_FileSystem::~M_FileSystem()
{
}

bool M_FileSystem::Init()
{
	//Devil init
	LOG(LogType::L_NORMAL, "DevIL Init");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	FileSystem::FSInit();
	MeshLoader::EnableDebugMode();

	return true;
}

bool M_FileSystem::Start()
{
	return true;
}

update_status M_FileSystem::Update(float dt)
{
	return update_status::UPDATE_CONTINUE;
}

bool M_FileSystem::CleanUp()
{

	FileSystem::FSDeInit();
	MeshLoader::DisableDebugMode();

	return true;
}
