#pragma once

#include "ModuleRenderer3D.h"
#include <vector>

class Mesh;
class aiScene;
class aiNode;
class aiMesh;
class GameObject;

enum class ImportType {
	NOTYPE = -1,
	MESH,
	TEXTURE,
};
//Change this to file system
namespace MeshLoader
{
	void FSInit();
	void FSDeInit();

	void EnableDebugMode();
	void DisableDebugMode();

	void logCallback(const char* message, char* user);

	void ImportFBX(const char* full_path, std::vector<Mesh*>& _mesh, GameObject* gmRoot);

	void NodeToGameObject(std::vector<Mesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName);
	Mesh* LoadMesh(aiMesh* importedMesh);

	//TODO: Not finished
	ImportType GetTypeFromPath(const char* path);

}

namespace StringLogic {

	std::string FileNameFromPath(const char* _path);

}

#define LIBRARY_PATH "Library/"
#define MESHES_PATH "Library/Meshes/"
#define MATERIALS_PATH "Library/Materials/"
#define TEXTURES_PATH "Library/Textures/"
//#define MODELS_PATH "Library/Models/"
//#define SCENES_PATH "Library/Scenes/"
namespace FileSystem
{
	void LoadFile(const char* globalPath);


	void CreateLibraryDirectories();

	// Utility functions
	bool AddPath(const char* path_or_zip);
	bool Exists(const char* file) /*const*/;
	bool CreateDir(const char* dir);
	bool IsDirectory(const char* file) /*const*/;
	const char* GetWriteDir() /*const*/;
	void DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) /*const*/;
	void GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) /*const*/;
	void GetRealDir(const char* path, std::string& output) /*const*/;
	std::string GetPathRelativeToAssets(const char* originalPath) /*const*/;

	bool HasExtension(const char* path) /*const*/;
	bool HasExtension(const char* path, std::string extension) /*const*/;
	bool HasExtension(const char* path, std::vector<std::string> extensions) /*const*/;

	std::string NormalizePath(const char* path) /*const*/;
	void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) /*const*/;

	// Open for Read/Write
	unsigned int Load(const char* path, const char* file, char** buffer) /*const*/;
	unsigned int Load(const char* file, char** buffer) /*const*/;

	bool DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath);
	bool DuplicateFile(const char* srcFile, const char* dstFile);

	unsigned int Save(const char* file, const void* buffer, unsigned int size, bool append = false) /*const*/;
	bool Remove(const char* file);

	uint64 GetLastModTime(const char* filename);
	std::string GetUniqueName(const char* path, const char* name) /*const*/;
}