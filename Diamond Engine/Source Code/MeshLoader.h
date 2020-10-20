#pragma once

#include "ModuleRenderer3D.h"
#include <vector>

class Mesh;

namespace MeshLoader
{

	void EnableDebugMode();
	void DisableDebugMode();

	void logCallback(const char* message, char* user);

	void ImportFBX(const char* full_path, std::vector<Mesh*>& _mesh, int temporalTexID);

}