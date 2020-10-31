#pragma once

#include "Module.h"

class M_FileSystem : public Module
{

public:
	M_FileSystem(Application* app, bool start_enabled = true);
	virtual ~M_FileSystem();

	bool Init() override;
	bool Start() override;

	update_status Update(float dt) override;

	bool CleanUp() override;

private:

	//void LoadFile(const char* globalPath);
	//ImportType GetTypeFromPath(const char* path);

	//void FSInit();
	//void FSDeInit();

	//void CreateLibraryDirectories();

	//// Utility functions
	//bool AddPath(const char* path_or_zip);
	//bool Exists(const char* file) const;
	//bool CreateDir(const char* dir);
	//bool IsDirectory(const char* file) const;

	//std::string NormalizePath(const char* path) const;
	//void SplitFilePath(const char* full_path, std::string* path, std::string* file = nullptr, std::string* extension = nullptr) const;

	//// Open for Read/Write
	//unsigned int Load(const char* path, const char* file, char** buffer) const;
	//unsigned int Load(const char* file, char** buffer) const;

	//uint Save(const char* file, char* buffer, uint size, bool append);
	//void GetFileName(const char* file, std::string& fileName, bool extension);
	//uint Copy(const char* file, const char* dir, std::string& outputFile);

	//bool Remove(const char* file);

};