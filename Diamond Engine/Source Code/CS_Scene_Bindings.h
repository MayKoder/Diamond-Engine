#pragma once

#include "Application.h"
#include"MO_Scene.h"


void CS_LoadScene(int libraryPath)
{
	EngineExternal->moduleScene->holdUID = libraryPath;
}