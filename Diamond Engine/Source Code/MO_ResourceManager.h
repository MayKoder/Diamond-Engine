#pragma once

#include "Module.h"
#include<map>

#include"MO_FileSystem.h"
#include"DEResource.h"

class M_ResourceManager : public Module
{

public:
	M_ResourceManager(Application* app, bool start_enabled = true);
	virtual ~M_ResourceManager();

	bool Init() override;
	bool Start() override;
	update_status PreUpdate(float dt) override;
	bool CleanUp() override;
	void OnGUI() override;


	int GenerateNewUID();
	int GetMetaUID(const char* metaFile);
	int ExistsOnLibrary(const char* file_in_assets) const;	
	int CreateLibraryFromAssets(const char* assetsFile);
	int ImportFile(const char* assetsFile, Resource::Type type);

	void PopulateFileArray();
	void UnloadResource(int uid);
	void RequestFromAssets(const char* assets_path);
	void GenerateMeta(const char* aPath, const char* lPath, unsigned int uid, Resource::Type type);

	Resource* RequestResource(int uid, const char* libraryPath = nullptr);
	Resource* CreateNewResource(const char* assetsFile, uint uid, Resource::Type type);
	Resource* LoadFromLibrary(const char* libraryFile, Resource::Type type, uint _uid);
	
	Resource::Type GetMetaType(const char* metaFile);
	Resource::Type GetTypeFromAssetExtension(const char* assetFile);
	Resource::Type GetTypeFromLibraryExtension(const char* libraryFile);

	std::string GetMetaPath(const char* assetsFile);
	std::string LibraryFromMeta(const char* metaFile);
	std::string GenLibraryPath(uint _uid, Resource::Type _type);

	void UpdateMeshesDisplay();
	void NeedsDirsUpdate(AssetDir& dir);

private:
	void LoadResource(int uid);
	void ReleaseResource(int uid);
	void UpdateFile(AssetDir& modDir);

	bool IsResourceLoaded(int uid);


private:
	std::map<int, Resource*> resources;

	float fileCheckTime;
	float fileUpdateDelay;

public:
	AssetDir assetsRoot;
	AssetDir meshesLibraryRoot;
};