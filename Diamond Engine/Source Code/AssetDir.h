#pragma once

#include<string>
#include"Globals.h"
#include<vector>

//TODO: Add full directory name to use as loading target
struct AssetDir
{
	AssetDir(const char* _dName, const char* _imPath, uint64 _lMod, bool _dir = false);
	~AssetDir();

	void ClearData();

	bool HasMeta();
	void GenerateMeta();
	//void ReadMeta();

	void GenerateMetaRecursive();
	void CreateLibraryFileRecursive();

	void GenerateMetaPath();

	void DeletePermanent();

	std::string dirName = "";
	std::string importPath = "";

	std::string metaFileDir = "";

	std::vector<AssetDir> childDirs;

	uint64 lastModTime = 0;
	bool isDir = false;
	uint metaUID;
};