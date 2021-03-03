#include"AssetDir.h"
#include"Application.h"
#include"MO_ResourceManager.h"
#include"IM_FileSystem.h"
#include"IM_ModelImporter.h"
#include"MO_MonoManager.h"
#include"DEJsonSupport.h"

AssetDir::AssetDir(const char* _dName, const char* _imPath, uint64 _lMod, bool _dir) : isDir(_dir), lastModTime(_lMod),
resourceType(Resource::Type::UNKNOWN), parentDir(nullptr)
{
	dirName = _dName;
	dirName.push_back('\0');
	importPath = _imPath;
	importPath.push_back('\0');

	if (!isDir) 
	{
		GenerateMetaPath();
		if (HasMeta()) 
		{
			LoadDataFromMeta();
		}
		else if(!isDir)
		{
			//resourceType = Resource::Type::MESH;
			resourceType = EngineExternal->moduleResources->GetTypeFromLibraryExtension(_imPath);
		}
	}
}

AssetDir::~AssetDir()
{
	dirName.clear();
	childDirs.clear();
	importPath.clear();
	metaFileDir.clear();
	libraryPath.clear();
	lastModTime = 0;
	parentDir = nullptr;
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

void AssetDir::GenerateMeta()
{
	GenerateMetaPath();

	if (!HasMeta())
	{
		this->resourceType = EngineExternal->moduleResources->GetTypeFromAssetExtension(importPath.c_str());
		this->metaUID = EngineExternal->moduleResources->GenerateNewUID();
		this->libraryPath = EngineExternal->moduleResources->GenLibraryPath(metaUID, resourceType).c_str();

		EngineExternal->moduleResources->GenerateMeta(importPath.c_str(), libraryPath.c_str(), metaUID, resourceType);

	}
}

void AssetDir::LoadDataFromMeta()
{
	JSON_Value* metaJSON = json_parse_file(metaFileDir.c_str());
	DEConfig rObj(json_value_get_object(metaJSON));

	metaUID = rObj.ReadInt("UID");
	resourceType = static_cast<Resource::Type>(rObj.ReadInt("Type"));
	libraryPath = rObj.ReadString("Library Path");

	//Free memory
	json_value_free(metaJSON);
}

void AssetDir::GenerateMetaRecursive()
{
	if (!isDir)
	{
		GenerateMeta();
	}

	for (size_t i = 0; i < childDirs.size(); i++)
	{
		childDirs[i].GenerateMetaRecursive();
	}
}

void AssetDir::CreateLibraryFileRecursive()
{
	//Create library file
	if (!isDir && EngineExternal->moduleResources->ExistsOnLibrary(importPath.c_str()) == 0)
	{
		//Create mirror
		uint id = EngineExternal->moduleResources->CreateLibraryFromAssets(importPath.c_str());
	}

	for (size_t i = 0; i < childDirs.size(); i++)
	{
		childDirs[i].CreateLibraryFileRecursive();
	}
}

void AssetDir::GenerateMetaPath()
{
	metaFileDir = importPath;
	metaFileDir.pop_back();

	metaFileDir += ".meta";
	metaFileDir.push_back('\0');
}

void AssetDir::DeletePermanent()
{
	//Directory can only be deleted if there is nothing inside
	if (isDir && childDirs.size() != 0) 
	{
		//Delete all files inside recursive
		for (size_t i = 0; i < childDirs.size(); i++)
		{
			childDirs[i].DeletePermanent();
		}
	}

	//Remove library
	if (!isDir) 
	{
		EngineExternal->moduleFileSystem->DeleteAssetFile(libraryPath.c_str());

		if (resourceType == Resource::Type::MODEL) 
		{
			std::vector<uint> meshesID;
			//Get meshes from fbx meta
			if (FileSystem::Exists(metaFileDir.c_str())) 
			{
				ModelImporter::GetMeshesFromMeta(importPath.c_str(), meshesID);

				//Delete said meshes
				for (int i = 0; i < meshesID.size(); i++)
				{
					std::string meshes = MESHES_PATH;
					meshes += std::to_string(meshesID[i]);
					meshes += ".mmh";
					EngineExternal->moduleFileSystem->DeleteAssetFile(meshes.c_str());
				}

				EngineExternal->moduleResources->UpdateMeshesDisplay();
			}		
		}
	}

	EngineExternal->moduleFileSystem->DeleteAssetFile(importPath.c_str());

	if (!isDir)
	{
		if (resourceType == Resource::Type::SCRIPT)
		{
			EngineExternal->moduleMono->RemoveScriptFromSLN(this->importPath.c_str());
			EngineExternal->moduleMono->ReCompileCS();
		}
	}

	EngineExternal->moduleFileSystem->DeleteAssetFile(metaFileDir.c_str());

	ClearData();
}
