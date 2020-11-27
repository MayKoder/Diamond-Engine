#include "MO_ResourceManager.h"
#include"Application.h"
#include"ImGui/imgui.h"

//Importers
#include"IM_FileSystem.h"
#include"IM_TextureImporter.h"
#include"IM_MeshLoader.h"
#include"IM_ModelImporter.h"

#include"Globals.h"

#include"RE_Texture.h"
#include"RE_Mesh.h"
#include"DEJsonSupport.h"
//#include"DEResource.h"

M_ResourceManager::M_ResourceManager(Application* app, bool start_enabled) : Module(app, start_enabled), rootFile("Assets", "Assets", 0, true),
fileCheckTime(0.f), fileUpdateDelay(2.f)
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
	rootFile.lastModTime = App->moduleFileSystem->GetLastModTime(rootFile.importPath.c_str());
	return true;
}

update_status M_ResourceManager::PreUpdate(float dt)
{
	fileCheckTime += dt;
	if (fileCheckTime >= fileUpdateDelay) 
	{
		NeedsDirsUpdate(rootFile);
		fileCheckTime = 0.f;
	}


	return update_status::UPDATE_CONTINUE;
}

bool M_ResourceManager::CleanUp()
{
	for (auto it = resources.begin(); it != resources.end(); ++it)
	{
		(*it).second->UnloadFromMemory();
		delete (*it).second;
	}
	resources.clear();

	return true;
}

void M_ResourceManager::OnGUI()
{
	//TODO: Move this to a new editor window
	if (ImGui::CollapsingHeader("Resource Display", ImGuiTreeNodeFlags_DefaultOpen))
	{
		for (auto it = resources.begin(); it != resources.end(); ++it)
		{
			ImGui::Text("%i: %s and %s", (*it).second->GetReferenceCount(), (*it).second->GetAssetPath(), (*it).second->GetAssetPath());
		}
	}
}

void M_ResourceManager::PopulateFileArray()
{
	Timer prof;
	prof.Start();

	//Dirs returns directories inside the folder so we should use some kind of recursive get until dirs is empty
	rootFile.ClearData();

	rootFile = AssetDir("Assets", "Assets", App->moduleFileSystem->GetLastModTime("Assets"),true);
	App->moduleFileSystem->GetAllFilesRecursive(rootFile);

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

		}


		//TODO: Then find modified files and create o recalulate .meta and library files
	}
}

Resource* M_ResourceManager::RequestResource(int uid)
{
	//Find if the resource is already loaded
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
	{
		it->second->IncreaseReferenceCount();
		return it->second;
	}
	////Find the library file (if exists) and load the custom file format
	//return TryToLoadResource();
}

//You should enter here only if the UID of the file on asset does not exist on library
int M_ResourceManager::ImportFile(const char* assetsFile, Resource::Type type)
{
	//To check if a resource is loaded we need to create .meta files first
	
	if (type == Resource::Type::UNKNOWN /*|| type == Resource::Type::MODEL*/)
		return 0; 

	Resource* resource = CreateNewResource(assetsFile, type); //Save ID, assetsFile path, libraryFile path

	int ret = 0;
	
	char* fileBuffer = nullptr;
	unsigned int size = FileSystem::LoadToBuffer(assetsFile, &fileBuffer); //<-- pseudocode, load from File System

	switch (resource->GetType()) 
	{
		case Resource::Type::TEXTURE: TextureImporter::Import(fileBuffer, size, resource); break;
		case Resource::Type::MODEL: ModelImporter::Import(fileBuffer, size, resource); break;
		//case Resource::Type::MESH: MeshLoader::BufferToMeshes(fileBuffer, size, resource); break;
		//case Resource::Type::SCENE: App->scene->Import(fileBuffer, resource); break;
	}

	//Save the resource to custom format
	ret = resource->GetUID();

	RELEASE_ARRAY(fileBuffer);

	//Generate meta
	GenerateMeta(*resource);

	//TODO: Uncomment this in the future
	//LoadResource(ret);
	//UnloadResource(ret);

	return ret;
}

int M_ResourceManager::CreateLibraryFromAssets(const char* assetsFile)
{
	LOG(LogType::L_WARNING, "File created from assets");
	uint resID = ImportFile(assetsFile, GetTypeFromAssetExtension(assetsFile));
	return resID;
}

Resource* M_ResourceManager::CreateNewResource(const char* assetsFile, Resource::Type type)
{
	Resource* ret = nullptr;

	int uid = GenerateNewUID();

	static_assert(static_cast<int>(Resource::Type::UNKNOWN) == 3, "Update all switches with new type");
	switch (type) 
	{
		case Resource::Type::TEXTURE: ret = (Resource*) new ResourceTexture(uid); break;
		case Resource::Type::MODEL: ret = (Resource*) new ResourceMesh(uid); break;
		//case Resource::Type::SCENE : ret = (Resource*) new ResourceScene(uid); break;
	}

	if (ret != nullptr)
	{
		resources[uid] = ret;
		ret->SetAssetsPath(assetsFile);
		ret->SetLibraryPath(GenLibraryPath(*ret).c_str());
	}

	return ret;
}

std::string M_ResourceManager::GenLibraryPath(Resource& res)
{
	std::string ret = "";
	std::string nameNoExt = "";

	FileSystem::GetFileName(res.GetAssetPath(), nameNoExt, false);

	switch (res.GetType())
	{
		case Resource::Type::TEXTURE: ret = MATERIALS_PATH; ret += nameNoExt; ret += ".dds"; break;
		case Resource::Type::MODEL: ret = MESHES_PATH; ret += nameNoExt; ret += ".mmh"; break;
		case Resource::Type::SCENE : ret = SCENES_PATH; ret += nameNoExt; ret += ".des"; break;
	}

	return ret;
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
	{
		res->UnloadFromMemory();
		resources.erase(it);
	}
}

bool M_ResourceManager::IsResourceLoaded(int uid)
{
	std::map<int, Resource*>::iterator it = resources.find(uid);
	if (it != resources.end())
		return true;

	return false;
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


	return Resource::Type::UNKNOWN;
}

void M_ResourceManager::GenerateMeta(Resource& res)
{
	//TODO: Move all json usage to own structure ASAP
	JSON_Value* file = json_value_init_object();
	DEConfig rObj(json_value_get_object(file));

	rObj.WriteString("Library Path", res.GetLibraryPath());
	rObj.WriteString("Assets Path", res.GetAssetPath());

	rObj.WriteInt("UID", res.GetUID());
	rObj.WriteInt("Type", (int)res.GetType());

	std::string path = res.GetAssetPath() + std::string(".meta");
	json_serialize_to_file_pretty(file, path.c_str());

	//Free memory
	json_value_free(file);
}

