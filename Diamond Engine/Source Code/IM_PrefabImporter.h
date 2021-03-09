#pragma once
#include "Globals.h"

class GameObject;

namespace PrefabImporter
{
	void SavePrefab(const char* folder, GameObject* gameObject);
	GameObject* LoadPrefab(uint UID);
}