#pragma once

#include "MO_Renderer3D.h"
#include <vector>

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};

namespace StringLogic {
	std::string GlobalToLocalPath(const char* _globalPath);
}



#define ASSETS_PATH "Assets/"

#define LIBRARY_PATH "Library/"
#define MESHES_PATH "Library/Meshes/"
#define TEXTURES_PATH "Library/Textures/"
#define MODELS_PATH "Library/Models/"
#define SCENES_PATH "Library/Scenes/"
#define SCRIPTS_PATH "Library/ScriptsAssembly/"
#define SHADERS_PATH "Library/Shaders/"
#define MATERIALS_PATH "Library/Materials/"
#define SOUNDS_PATH "Library/Sounds/"
#define ANIMATIONS_PATH "Library/Animations/"

namespace FileSystem
{
	ImportType GetTypeFromPath(const char* path);

	void FSInit();
	void FSDeInit();
	void CreateLibraryFolders();
	void LoadDroppedFile(const char* globalPath);
	void GetFileName(const char* file, std::string& fileName, bool extension);
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) /*const*/;

	// Utility functions
	bool Exists(const char* file) /*const*/;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) /*const*/;
	bool AddPath(const char* path_or_zip);
	int Delete(const char* file_to_delete); //Deletes a file if it exists. Returns nonzero on success, zero on failure

	std::string NormalizePath(const char* path) /*const*/;
	std::string UnNormalizePath(const char* full_path);


	// Open for Read/Write
	unsigned int LoadToBuffer(const char* file, char** buffer) /*const*/;
	unsigned int Load(const char* path, const char* file, char** buffer) /*const*/;

	uint Save(const char* file, char* buffer, uint size, bool append);
	uint Copy(const char* file, const char* dir, std::string& outputFile);

	//std::string FileToText(const char* path);

	//TEMPORAL FIX
	void CreateLibrarySoundBanks();
}