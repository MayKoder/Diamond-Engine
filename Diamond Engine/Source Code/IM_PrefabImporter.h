#pragma once
#include "Globals.h"

class GameObject;
#include"parson/parson.h"

namespace PrefabImporter
{
	void SavePrefab(const char* folder, GameObject* gameObject);
	GameObject* LoadPrefab(const char* libraryPath);
	GameObject* LoadGOData(JSON_Object* goJsonObj, GameObject* parent);
}