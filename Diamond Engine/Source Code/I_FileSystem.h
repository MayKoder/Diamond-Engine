#pragma once

#include "ModuleRenderer3D.h"
#include <vector>

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};

namespace StringLogic {

	std::string FileNameFromPath(const char* _path);
	std::string GlobalToLocalPath(const char* _globalPath);
}

#define ASSETS_PATH "Assets/"

#define LIBRARY_PATH "Library/"
#define MESHES_PATH "Library/Meshes/"
#define MATERIALS_PATH "Library/Materials/"
//#define TEXTURES_PATH "Library/Textures/"
//#define MODELS_PATH "Library/Models/"
#define SCENES_PATH "Library/Scenes/"
namespace FileSystem
{
	void LoadDroppedFile(const char* globalPath);
	ImportType GetTypeFromPath(const char* path);

	void FSInit();
	void FSDeInit();

	void CreateLibraryDirectories();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) /*const*/;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) /*const*/;

	//void GetRealDir(const char* path, std::string& output);
	//std::string GetPathRelativeToAssets(const char* originalPath) /*const*/;

	std::string NormalizePath(const char* path) /*const*/;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) /*const*/;

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) /*const*/;
	unsigned int LoadToBuffer(const char* file, char** buffer) /*const*/;

	uint Save(const char* file, char* buffer, uint size, bool append);
	void GetFileName(const char* file, std::string& fileName, bool extension);
	uint Copy(const char* file, const char* dir, std::string& outputFile);

	bool Remove(const char* file);
}