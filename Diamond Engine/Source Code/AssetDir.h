#pragma once

#include<string>
#include"Globals.h"
#include<vector>
#include"DEResource.h"

struct AssetDir
{
	AssetDir(const char* _dName, const char* _imPath, uint64 _lMod, bool _dir = false);
	~AssetDir();

	void ClearData();

	bool HasMeta();
	void GenerateMeta();
	void LoadDataFromMeta();
	//void ReadMeta();

	void GenerateMetaRecursive();
	void CreateLibraryFileRecursive();

	void GenerateMetaPath();

	void DeletePermanent();

	std::string dirName = ""; //File name

	std::string importPath = ""; //Assets path
	std::string metaFileDir = ""; //Meta file dir 

	std::string libraryPath; //Linked library path

	std::vector<AssetDir> childDirs; //If it's a folder, files inside

	uint64 lastModTime = 0;
	bool isDir = false;
	uint metaUID;
	Resource::Type resourceType;

	AssetDir* parentDir;
};