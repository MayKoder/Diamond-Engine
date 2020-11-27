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

	void PopulateFileArray();


	//Change int to UID?
	int ExistsOnLibrary(const char* file_in_assets) const;
	
	int ImportFile(const char* assetsFile, Resource::Type type);
	int CreateLibraryFromAssets(const char* assetsFile);

	int GenerateNewUID();


private:
	void NeedsDirsUpdate(AssetDir& dir);

	//const Resource* RequestResource(int uid) const;
	Resource* RequestResource(int uid);
	void ReleaseResource(int uid);

	Resource* CreateNewResource(const char* assetsFile, Resource::Type type);
	std::string GenLibraryPath(Resource& res);

	void LoadResource(int uid);
	void UnloadResource(int uid);

	bool IsResourceLoaded(int uid);
	Resource::Type GetTypeFromAssetExtension(const char* assetFile);

	void GenerateMeta(Resource& res);

private:
	std::map<int, Resource*> resources;

	float fileCheckTime;
	float fileUpdateDelay;

public:
	AssetDir rootFile;
};