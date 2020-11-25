#pragma once

#include "Module.h"
#include<vector>
#include<string>

//TODO: Add full directory name to use as loading target
struct AssetDir 
{
	AssetDir(const char* _dName, const char* _imPath, bool _dir = false);
	~AssetDir();

	void ClearData();

	std::string dirName = "";
	std::string importPath = "";

	std::vector<AssetDir> childDirs;
	bool isDir = false;
};

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

};