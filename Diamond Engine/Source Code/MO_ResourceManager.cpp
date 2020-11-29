#include "MO_ResourceManager.h"
#include"Application.h"
#include"ImGui/imgui.h"

//Importers
#include"IM_FileSystem.h"
#include"IM_TextureImporter.h"
#include"IM_MeshLoader.h"
#include"IM_ModelImporter.h"

#include"MO_Scene.h"

#include"Globals.h"

#include"RE_Texture.h"
#include"RE_Mesh.h"
#include"DEJsonSupport.h"
//#include"DEResource.h"

M_ResourceManager::M_ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled), assetsRoot("Assets", "Assets", 0, true),
fileCheckTime(0.f), fileUpdateDelay(2.f), meshesLibraryRoot("Meshes", "Library/Meshes", 0, true)
{
}

M_ResourceManager::~M_ResourceManager()
{
}

bool M_ResourceManager::Init()
{
	return true;
}

bool M_ResourceManager::Start()
{
	assetsRoot.lastModTime = App->moduleFileSystem->GetLastModTime(assetsRoot.importPath.c_str());
	meshesLibraryRoot.lastModTime = App->moduleFileSystem->GetLastModTime(meshesLibraryRoot.importPath.c_str());
	return true;
}

update_status M_ResourceManager::PreUpdate(float dt)
{
	fileCheckTime += dt;
	if (fileCheckTime >= fileUpdateDelay) 
	{
		NeedsDirsUpdate(assetsRoot);
	}
	return update_status::UPDATE_CONTINUE;
}

bool M_ResourceManager::CleanUp()
{
	return true;
}

void M_ResourceManager::OnGUI()
{
	//TODO: Move this to a new editor window
	if (ImGui::CollapsingHeader("Resource Display", ImGuiTreeNodeFlags_DefaultOpen))
	{
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%i resources loaded", resources.size());
		for (auto it = resources.begin(); it != resources.end(); ++it)
		{
			ImGui::Text("%i: %i and %s", (*it).second->GetReferenceCount(), (*it).second->GetUID(), (*it).second->GetLibraryPath());
		}
	}
}

void M_ResourceManager::PopulateFileArray()
{
	Timer prof;
	prof.Start();

	//Dirs returns directories inside the folder so we should use some kind of recursive get until dirs is empty
	assetsRoot.ClearData();

	assetsRoot = AssetDir("Assets", "Assets", App->moduleFileSystem->GetLastModTime("Assets"),true);
	App->moduleFileSystem->GetAllFilesRecursive(assetsRoot);

	LOG(LogType::L_WARNING, "Took %f seconds to calculate assets", ((float)prof.Read() / 1000.f));
}

int M_ResourceManager::ExistsOnLibrary(const char* file_in_assets) const
{
	std::string metaFile(file_in_assets);
	metaFile += ".meta";

	if (FileSystem::Exists(metaFile.c_str())) 
	{
		//TODO: Move all json usage to own structure ASAP
		JSON_Value* metaJSON = json_parse_file(metaFile.c_str());
		DEConfig rObj(json_value_get_object(metaJSON));

		std::string libPath = rObj.ReadString("Library Path");

		if (FileSystem::Exists(libPath.c_str()))
			return rObj.ReadInt("UID");

		//Free memory
		json_value_free(metaJSON);
	}


	return 0;
}

int M_ResourceManager::GenerateNewUID()
{
	return App->GetRandomInt();
}

void M_ResourceManager::NeedsDirsUpdate(AssetDir& dir)
{
	if (dir.lastModTime == App->moduleFileSystem->GetLastModTime(dir.importPath.c_str())) 
	{
		for (unsigned int i = 0; i < dir.childDirs.size(); i++)
		{
			NeedsDirsUpdate(dir.childDirs[i]);
		}
	}
	else
	{

		//If its a dir, recalculate all the new folders
		if (dir.isDir) 
		{
			LOG(LogType::L_WARNING, "Dir %s updated", dir.importPath.c_str());
			dir.childDirs.clear();
			dir.lastModTime = App->moduleFileSystem->GetLastModTime(dir.importPath.c_str());

			App->moduleFileSystem->GetAllFilesRecursive(dir);

			for (size_t i = 0; i < dir.childDirs.size(); i++)
			{
				AssetDir& modFile = dir.childDirs[i];
				if (!dir.childDirs[i].HasMeta()) 
				{
					modFile.GenerateMeta();
					if(!this->ExistsOnLibrary(modFile.importPath.c_str()))
						this->ImportFile(modFile.importPath.c_str(), this->GetMetaType(modFile.metaFileDir.c_str()));
					LOG(LogType::L_NORMAL, "Meta generated");
				}
				else if(modFile.lastModTime != App->moduleFileSystem->GetLastModTime(modFile.importPath.c_str()))
				{
					UpdateFile(dir);
				}
				//TODO IMPORTANT: If file is updated, update library but not meta
			}

		}

		//TODO: Then find modified files and create o recalulate .meta and library files
		//SUPER IMPOIRTANT TODO DONT FFORGET, REGENERATE LIBRARY FILE BUT NEVER MODIFY META FILE
		//Check if this works
		if (!dir.isDir) 
		{
			UpdateFile(dir);
		}
	}
	fileCheckTime = 0.f;
}

void M_ResourceManager::UpdateMeshesDisplay()
{
	meshesLibraryRoot.childDirs.clear();
	App->moduleFileSystem->GetAllFilesRecursive(meshesLibraryRoot);
	LOG(LogType::L_WARNING, "Mesh display updated");
}

//Returns a resource* if the resource is loaded or creates a new resource from the library file
Resource* M_ResourceManager::RequestResource(int uid, const char* libraryPath)
{
	//Find if the resource is already loaded
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		it->second->IncreaseReferenceCount();
		return it->second;
	}

	//Find the library file (if exists) and load the custom file format
	if (libraryPath != nullptr) 
	{
		Resource* ret = nullptr;

		static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 4, "Update all switches with new type");

		//Save check
		if (FileSystem::Exists(libraryPath))
		{
			switch (GetTypeFromLibraryExtension(libraryPath))
			{
				case Resource::Type::TEXTURE: ret = (Resource*) new ResourceTexture(uid); break;
				//case Resource::Type::MODEL: ret = (Resource*) new ResourceMesh(uid); break;
				case Resource::Type::MESH: ret = (Resource*) new ResourceMesh(uid); break;
				//case Resource::Type::SCENE : ret = (Resource*) new ResourceScene(uid); break;
			}

			if (ret != nullptr)
			{
				resources[uid] = ret;
				ret->SetAssetsPath("");
				ret->SetLibraryPath(libraryPath);
				ret->IncreaseReferenceCount();

				ret->LoadToMemory();
			}
		}
		return ret;
	}

	return nullptr;
	//return TryToLoadResource();
}

void M_ResourceManager::ReleaseResource(int uid)
{
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;
	
	Resource* res = (*it).second;
	(*it).second->UnloadFromMemory();
	resources.erase((*it).second->GetUID());
	delete res;
}

//You should enter here only if the UID of the file on asset does not exist on library
int M_ResourceManager::ImportFile(const char* assetsFile, Resource::Type type)
{
	//To check if a resource is loaded we need to create .meta files first
	
	if (type == Resource::Type::UNKNOWN /*|| type == Resource::Type::MODEL*/)
		return 0; 

	//Generate meta
	std::string meta = M_ResourceManager::GetMetaPath(assetsFile);
	uint resUID = GetMetaUID(meta.c_str());

	Resource* resource = CreateNewResource(assetsFile, resUID, type); //Save ID, assetsFile path, libraryFile path

	int ret = 0;
	
	char* fileBuffer = nullptr;
	unsigned int size = FileSystem::LoadToBuffer(assetsFile, &fileBuffer); //<-- pseudocode, load from File System

	switch (resource->GetType()) 
	{
		case Resource::Type::TEXTURE: TextureImporter::Import(fileBuffer, size, resource); break;
		case Resource::Type::MODEL: ModelImporter::Import(fileBuffer, size, resource); break;
		//case Resource::Type::MESH: MeshLoader::BufferToMeshes(fileBuffer, size, resource); break;
		case Resource::Type::SCENE: FileSystem::Save(resource->GetLibraryPath(), fileBuffer, size, false); break;
	}

	//Save the resource to custom format
	ret = resource->GetUID();

	RELEASE_ARRAY(fileBuffer);
	UnloadResource(ret);

	return ret;
}

int M_ResourceManager::CreateLibraryFromAssets(const char* assetsFile)
{
	LOG(LogType::L_WARNING, "File created from assets");
	uint resID = ImportFile(assetsFile, GetTypeFromAssetExtension(assetsFile));
	return resID;
}

void M_ResourceManager::RequestFromAssets(const char* assets_path)
{
	if (ExistsOnLibrary(assets_path) != 0)
	{
		std::string meta = GetMetaPath(assets_path);
		JSON_Value* scene = json_parse_file(meta.c_str());

		if (scene != NULL) 
		{
			DEConfig sceneObj(json_value_get_object(scene));

			switch ((Resource::Type)sceneObj.ReadInt("Type"))
			{
				case Resource::Type::TEXTURE: RequestResource(sceneObj.ReadInt("UID"), sceneObj.ReadString("Library Path")); break;
				case Resource::Type::MODEL: ModelImporter::LoadModelCustom(sceneObj.ReadString("Library Path")); break;
			}
			//Free memory
			json_value_free(scene);
		}
	}
	else
	{
		LOG(LogType::L_ERROR, "ASSET META OR LIBRARY NOT CREATED");
	}
}

Resource* M_ResourceManager::CreateNewResource(const char* assetsFile, uint uid, Resource::Type type)
{
	Resource* ret = nullptr;

	static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 4, "Update all switches with new type");
	switch (type) 
	{
		case Resource::Type::SCENE : ret = new Resource(uid, Resource::Type::SCENE); break;
		case Resource::Type::TEXTURE: ret = (Resource*) new ResourceTexture(uid); break;
		case Resource::Type::MODEL: ret = new Resource(uid, Resource::Type::MODEL); break;
		case Resource::Type::MESH: ret = (Resource*) new ResourceMesh(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->SetAssetsPath(assetsFile);
		ret->SetLibraryPath(GenLibraryPath(ret->GetUID(), type).c_str());
		ret->IncreaseReferenceCount();
	}

	return ret;
}

Resource* M_ResourceManager::LoadFromLibrary(const char* libraryFile, Resource::Type type, uint _uid)
{
	Resource* ret = nullptr;

	static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 4, "Update all switches with new type");

	int uid = _uid;
	switch (type)
	{
		case Resource::Type::TEXTURE: ret = (Resource*) new ResourceTexture(uid); break;
		case Resource::Type::MODEL: ret = (Resource*) new ResourceMesh(uid); break;
		case Resource::Type::MESH: ret = (Resource*) new ResourceMesh(uid); break;
		//case Resource::Type::SCENE : ret = (Resource*) new ResourceScene(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->SetAssetsPath("");
		ret->SetLibraryPath(libraryFile);
		ret->IncreaseReferenceCount();
	}

	return ret;
}

int M_ResourceManager::GetMetaUID(const char* metaFile)
{
	JSON_Value* metaJSON = json_parse_file(metaFile);
	DEConfig rObj(json_value_get_object(metaJSON));

	uint mUID = rObj.ReadInt("UID");

	//Free memory
	json_value_free(metaJSON);

	return mUID;
}

std::string M_ResourceManager::GenLibraryPath(uint _uid, Resource::Type _type)
{
	std::string ret = "";
	std::string nameNoExt = std::to_string(_uid);

	//FileSystem::GetFileName(res.GetAssetPath(), nameNoExt, false);

	switch (_type)
	{
		case Resource::Type::TEXTURE: ret = MATERIALS_PATH; ret += nameNoExt; ret += ".dds"; break;
		case Resource::Type::MODEL: ret = MODELS_PATH; ret += nameNoExt; ret += ".model"; break;
		case Resource::Type::MESH: ret = MESHES_PATH; ret += nameNoExt; ret += ".mmh"; break;
		case Resource::Type::SCENE : ret = SCENES_PATH; ret += nameNoExt; ret += ".des"; break;
	}

	return ret;
}

std::string M_ResourceManager::GetMetaPath(const char* assetsFile)
{
	std::string metaFile(assetsFile);
	metaFile += ".meta";
	return metaFile;
}

Resource::Type M_ResourceManager::GetMetaType(const char* metaFile)
{
	JSON_Value* metaJSON = json_parse_file(metaFile);
	DEConfig rObj(json_value_get_object(metaJSON));

	//TODO: Change this casts with xxx_cast<>
	Resource::Type mUID = (Resource::Type)rObj.ReadInt("Type");

	//Free memory
	json_value_free(metaJSON);

	return mUID;
}

std::string M_ResourceManager::LibraryFromMeta(const char* metaFile)
{
	JSON_Value* metaJSON = json_parse_file(metaFile);

	if (metaJSON == nullptr)
		return "";

	DEConfig rObj(json_value_get_object(metaJSON));

	std::string libPath = rObj.ReadString("Library Path");

	//Free memory
	json_value_free(metaJSON);

	return libPath;
}

void M_ResourceManager::LoadResource(int uid)
{
	Resource* res = nullptr;

	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;

	res = it->second;
	res->IncreaseReferenceCount();

	//Load to memory [Buffers etcetc]
	res->LoadToMemory();

}

void M_ResourceManager::UnloadResource(int uid)
{
	Resource* res = nullptr;
	
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it == resources.end())
		return;
	
	res = it->second;
	res->DecreaseReferenceCount();

	if (res->GetReferenceCount() <= 0) 
		ReleaseResource(res->GetUID());

}

bool M_ResourceManager::IsResourceLoaded(int uid)
{
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return true;

	return false;
}

void M_ResourceManager::UpdateFile(AssetDir& modDir)
{
	LOG(LogType::L_WARNING, "File %s was modified, reimporting", modDir.dirName.c_str());
	ImportFile(modDir.importPath.c_str(), App->moduleResources->GetMetaType(modDir.metaFileDir.c_str()));
	modDir.lastModTime = App->moduleFileSystem->GetLastModTime(modDir.importPath.c_str());
}

Resource::Type M_ResourceManager::GetTypeFromAssetExtension(const char* assetFile)
{
	std::string ext(assetFile);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx" || ext == "dae")
		return Resource::Type::MODEL;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "dds")
		return Resource::Type::TEXTURE;
	if (ext == "des")
		return Resource::Type::SCENE;


	return Resource::Type::UNKNOWN;
}

Resource::Type M_ResourceManager::GetTypeFromLibraryExtension(const char* libraryFile)
{
	std::string ext(libraryFile);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "dds")
		return Resource::Type::TEXTURE;
	if (ext == "model")
		return Resource::Type::MODEL;
	if (ext == "mmh")
		return Resource::Type::MESH;
	if (ext == "des")
		return Resource::Type::SCENE;

	return Resource::Type::UNKNOWN;
}

void M_ResourceManager::GenerateMeta(const char* aPath, const char* lPath, unsigned int uid, Resource::Type type)
{
	//TODO: Move all json usage to own structure ASAP
	JSON_Value* file = json_value_init_object();
	DEConfig rObj(json_value_get_object(file));

	rObj.WriteString("Assets Path", aPath);
	rObj.WriteString("Library Path", lPath);

	rObj.WriteInt("UID", uid);
	rObj.WriteInt("Type", (int)type);

	std::string path = aPath + std::string(".meta");
	json_serialize_to_file_pretty(file, path.c_str());

	//Free memory
	json_value_free(file);
}

