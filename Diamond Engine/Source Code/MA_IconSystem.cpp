#include"Application.h"
#include "MA_IconSystem.h"
#include"MO_ResourceManager.h"
#include "RE_Texture.h"
#include"Globals.h"
#include<assert.h>

IconManager::IconManager(): iconCount(0)
{
}

IconManager::~IconManager()
{
	CleanUp();
}

void IconManager::CleanUp()
{
	for (auto it = icons.begin(); it != icons.end(); it++)
	{
		if (it->second != nullptr)
			EngineExternal->moduleResources->UnloadResource(it->second->GetUID());
	}
	icons.clear();
}

void IconManager::LoadPreDefinedIcons()
{
	LoadEditorIcon("PLAY", "EngineIcons/PlayButton.dds");
	LoadEditorIcon("STOP", "EngineIcons/StopButton.dds");
	LoadEditorIcon("PAUSE", "EngineIcons/PauseButton.dds");
	LoadEditorIcon("STEP", "EngineIcons/StepButton.dds");
	LoadEditorIcon("WARNING", "EngineIcons/Warning.dds");
	LoadEditorIcon("ERROR", "EngineIcons/Error.dds");
	LoadEditorIcon("INFO", "EngineIcons/Info.dds");
	LoadEditorIcon("FOLDER", "EngineIcons/Folder.dds");
	LoadEditorIcon("MODELS", "EngineIcons/Models.dds");
	LoadEditorIcon("TEXTURE", "EngineIcons/texture.dds");
	LoadEditorIcon("SCRIPT", "EngineIcons/cs.dds");
	LoadEditorIcon("SCENE", "EngineIcons/scene.dds");
	LoadEditorIcon("SHADER", "EngineIcons/shaders.dds");
	LoadEditorIcon("FONT", "EngineIcons/Font.dds");
}

void IconManager::LoadEditorIcon(const char* iconID, const char* path)
{
	//TODO: Maybe hard-coding id's is stupid, find a better way
	icons[iconID] = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(iconCount + 1, path));
	iconCount++;
}

GLuint IconManager::GetIconTextureID(const char* iconID)
{
	assert(iconID != nullptr, "iconID was not valid"); //Maybe remove some asserts...?
	return icons[iconID]->textureID;
}

GLuint IconManager::GetIconTextureID(Resource::Type iconType)
{
	const char* ret = nullptr;
	switch (iconType)
	{
	case Resource::Type::TEXTURE:
		ret = "TEXTURE";
		break;
	case Resource::Type::MODEL:
		ret = "MODELS";
		break;
	case Resource::Type::MESH:
		ret = "MODELS";
		break;
	case Resource::Type::SCENE:
		ret = "SCENE";
		break;
	case Resource::Type::SCRIPT:
		ret = "SCRIPT";
		break;
	case Resource::Type::SHADER:
		ret = "SHADER";
		break;
	/*case Resource::Type::MATERIAL:
		ret = "MATERIAL";
		break;
	case Resource::Type::ANIMATION:
		ret = "ANIMATION";
		break;
	*/
	case Resource::Type::FONT:
		ret = "FONT";
		break;
	case Resource::Type::UNKNOWN:
		ret = "FOLDER";
		break;
	default:
		ret = "MODELS";
		break;
	}

	return GetIconTextureID(ret);
}


