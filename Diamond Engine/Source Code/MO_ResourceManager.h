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

#ifndef STANDALONE
	bool Start() override;
	update_status PreUpdate(float dt) override;
#endif // !STANDALONE

	bool CleanUp() override;

#ifndef STANDALONE
	void OnGUI() override;
	Resource* RequestFromAssets(const char* assets_path);
	bool RenameAsset(const char* old_assets_path, const char* new_assets_path, char* buffer, uint size, Resource* resource);
#endif // !STANDALONE


	int GenerateNewUID();
	int GetMetaUID(const char* metaFile) const;
	int ExistsOnLibrary(const char* file_in_assets) const;	
	int CreateLibraryFromAssets(const char* assetsFile);
	int ImportFile(const char* assetsFile, Resource::Type type);

	void PopulateFileArray();
	void UnloadResource(int uid, bool releaseAtZero = true);
	void GenerateMeta(const char* aPath, const char* lPath, unsigned int uid, Resource::Type type);

	Resource* RequestResource(int uid, Resource::Type type);
	Resource* RequestResource(int uid, const char* libraryPath = nullptr);
	Resource* CreateNewResource(const char* assetsFile, uint uid, Resource::Type type);
	Resource* LoadFromLibrary(const char* libraryFile, Resource::Type type, uint _uid);
	Resource* GetResourceFromUID(int uid);
	
	Resource::Type GetMetaType(const char* metaFile) const;
	Resource::Type GetTypeFromAssetExtension(const char* assetFile) const;
	Resource::Type GetTypeFromLibraryExtension(const char* libraryFile) const;

	std::string GetMetaPath(const char* assetsFile);
	std::string LibraryFromMeta(const char* metaFile);
	std::string GenLibraryPath(uint _uid, Resource::Type _type);

	void UpdateMeshesDisplay();
	void UpdateAnimationsDisplay();
	void NeedsDirsUpdate(AssetDir& dir);

	void ZeroReferenceCleanUp();

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
	AssetDir animationsLibraryRoot;
};