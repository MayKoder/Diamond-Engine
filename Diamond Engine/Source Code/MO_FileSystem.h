#pragma once

#include "Module.h"
#include<vector>
#include<string>
#include"Globals.h"

#include"AssetDir.h"

class M_FileSystem : public Module
{

public:
	M_FileSystem(Application* app, bool start_enabled = true);
	virtual ~M_FileSystem();

	bool Init() override;
	bool Start() override;

	bool CleanUp() override;

public:
	void GetAllFiles(const char* directory, std::vector<AssetDir>& file_list);
	void GetAllFilesRecursive(AssetDir& _file);
	uint64 GetLastModTime(const char* filename);

	//Permanent delete, use with caution
	int DeleteAssetFile(const char* fileDir);

private:
	void GenerateAllMetaFiles();
	void ImportAssetsToLibrary();
};