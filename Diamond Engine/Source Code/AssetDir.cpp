#include"AssetDir.h"
#include"Application.h"
#include"MO_ResourceManager.h"
#include"IM_FileSystem.h"

AssetDir::AssetDir(const char* _dName, const char* _imPath, uint64 _lMod, bool _dir) : isDir(_dir), lastModTime(_lMod)
{
	dirName = _dName;
	dirName.push_back('\0');
	importPath = _imPath;
	importPath.push_back('\0');

	if (!isDir)
	{
		GenerateMetaPath();

		if (EngineExternal->moduleResources->ExistsOnLibrary(importPath.c_str()) == 0) /*TODO: Or has meta but the mirror on library does not exist*/
		{
			//Create mirror
			uint id = EngineExternal->moduleResources->CreateLibraryFromAssets(importPath.c_str());
		}
	}
}

AssetDir::~AssetDir()
{
	dirName.clear();
	childDirs.clear();
	importPath.clear();
	metaFileDir.clear();
	lastModTime = 0;
}

void AssetDir::ClearData()
{
	dirName.clear();
	childDirs.clear();
	isDir = false;
	importPath.clear();
	metaFileDir.clear();
	lastModTime = 0;
}

bool AssetDir::HasMeta()
{
	return FileSystem::Exists(metaFileDir.c_str());
}

void AssetDir::WriteMeta()
{
	//JSON_Value* file = json_value_init_object();

	//DEJson::write


	//json_serialize_to_file_pretty(file, metaFileDir.c_str());

	////Free memory
	//json_value_free(file);
}

void AssetDir::GenerateMetaPath()
{
	metaFileDir = importPath;
	metaFileDir.pop_back();

	metaFileDir += ".meta";
	metaFileDir.push_back('\0');
}