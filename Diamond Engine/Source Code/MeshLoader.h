#pragma once

#include "ModuleRenderer3D.h"
#include <vector>

namespace MeshLoader
{

	void EnableDebugMode();
	void DisableDebugMode();

	void myCallback(const char* msg, char* userData);

	void ImportFBX(const char* full_path, std::vector<Mesh*>& _mesh);

}