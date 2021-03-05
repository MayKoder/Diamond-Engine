#include "IM_FileSystem.h"
#include"IM_TextureImporter.h"

#include "Globals.h"

#include "Application.h"
#include "MO_Scene.h"

#include <fstream>
#include <iostream>
#include <filesystem>
#include <sstream> //std::stringstream

#include "PhysFS/include/physfs.h"
#include "IM_MeshLoader.h"
#include"GameObject.h"
#include"CO_Material.h"

#include"MO_Editor.h"
#include"MO_ResourceManager.h"

#include"RE_Texture.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

/*Convert global path to local path*/
std::string StringLogic::GlobalToLocalPath(const char* _globalPath)
{
	std::string localPath = FileSystem::NormalizePath(_globalPath);

	size_t pos = 0;
	pos = localPath.find(ASSETS_PATH);
	if (pos != std::string::npos)
	{
		localPath = localPath.substr(pos);
		if (localPath.c_str() != "")
			return localPath;
	}

	localPath.clear();
	return localPath;
}

/*Get the type of importer using any path with a file extension*/
ImportType FileSystem::GetTypeFromPath(const char* path)
{
	std::string ext(path);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx" || ext == "dae")
		return ImportType::MESH;
	if (ext == "tga" || ext == "png" || ext == "jpg" || ext == "dds")
		return ImportType::TEXTURE;

	return ImportType::NOTYPE;
}

/*Init PhysFS*/
void FileSystem::FSInit()
{
	// needs to be created before Init so other modules can use it
	char* base_path = SDL_GetBasePath();
	PHYSFS_init(nullptr);
	SDL_free(base_path);

	//Setting the working directory as the writing directory
	if (PHYSFS_setWriteDir(".") == 0)
		LOG(LogType::L_NORMAL, "File System error while creating write dir: %s\n", PHYSFS_getLastError());

	FileSystem::AddPath("."); //Adding ProjectFolder (working directory)
	FileSystem::AddPath("Assets");
	//FileSystem::AddPath("Assets/Primitives");
	FileSystem::CreateLibraryFolders();
}

void FileSystem::FSDeInit()
{
	PHYSFS_deinit();
}

//TODO: Move this to resource manager
void FileSystem::LoadDroppedFile(const char* globalPath)
{
	ImportType iType = GetTypeFromPath(globalPath);

	if (iType == ImportType::NOTYPE) {
		LOG(LogType::L_ERROR, "File extension not supported yet");
		return;
	}


	std::string normalizedPath = NormalizePath(globalPath);
	std::string relativePath = StringLogic::GlobalToLocalPath(globalPath);

	std::string output = "";

	std::string fileNameAndExtension = StringLogic::GlobalToLocalPath(normalizedPath.c_str());
	if (fileNameAndExtension.length() == 0)
		fileNameAndExtension = normalizedPath;

	if (Exists(fileNameAndExtension.c_str()) == 0)
	{
		Copy(globalPath, ASSETS_PATH, output);
		fileNameAndExtension = output;
	}

	//UPDATE ASSETS WINDOW WITH NEW FILE, JUST RECALCULATE ALL FOR NOW
	//TODO: Generate META FILE ASAP WHEN DROPPED
	//EngineExternal->moduleResources->GenerateMeta();
	std::string name = "";
	GetFileName(fileNameAndExtension.c_str(), name, true);

	AssetDir nFile(name.c_str(), fileNameAndExtension.c_str(), EngineExternal->moduleFileSystem->GetLastModTime(fileNameAndExtension.c_str()), false);
	nFile.GenerateMeta();
	nFile.CreateLibraryFileRecursive();

	EngineExternal->moduleResources->NeedsDirsUpdate(EngineExternal->moduleResources->assetsRoot);
}

void FileSystem::CreateLibraryFolders()
{
	CreateDir(LIBRARY_PATH);
	CreateDir(MESHES_PATH);
	CreateDir(MODELS_PATH);
	CreateDir(TEXTURES_PATH);
	CreateDir(SCENES_PATH);
	CreateDir(SCRIPTS_PATH);
	CreateDir(SHADERS_PATH);
	CreateDir(MATERIALS_PATH);
	CreateDir(ANIMATIONS_PATH);
	CreateDir(SOUNDS_PATH);

	CreateLibrarySoundBanks();//TODO move this somewhere else? ask myke
}

// Add a new zip file or folder
bool FileSystem::AddPath(const char* path_or_zip)
{
	bool ret = false;

	if (PHYSFS_mount(path_or_zip, nullptr, 1) == 0)
	{
		LOG(LogType::L_ERROR, "File System error while adding a path or zip: %s\n", PHYSFS_getLastError());
	}
	else
		ret = true;

	return ret;
}

//Deletes a file if it exists. Returns nonzero on success, zero on failure
int FileSystem::Delete(const char* file_to_delete)
{
	int success = 0;

	//Check if the file exists to avoid unnecessary operations and get the actual error
	if (!Exists(file_to_delete)) 
	{
		LOG(LogType::L_ERROR, " File %s could not be deleted, it does not exist");
		return 0;
	}

	success = PHYSFS_delete(file_to_delete); 

	//if it exists try to delete it
	if (success == 0) {
		LOG(LogType::L_ERROR, "File %s could not be deleted", file_to_delete);}
	else {
		LOG(LogType::L_NORMAL, "File %s was deleted successfully", file_to_delete);}

	return success;
}

// Check if a file exists
bool FileSystem::Exists(const char* file) /*const*/
{
	return PHYSFS_exists(file) != 0;
}

bool FileSystem::CreateDir(const char* dir)
{
	if (IsDirectory(dir) == false)
	{
		PHYSFS_mkdir(dir);
		return true;
	}
	return false;
}

// Check if a file is a directory
bool FileSystem::IsDirectory(const char* file) /*const*/
{
	return PHYSFS_isDirectory(file) != 0;
}

//*PHYSFS_getSearchPath() not working and causing a crash
//void FileSystem::GetRealDir(const char* path, std::string& output)
//{
//	output = PHYSFS_getBaseDir();
//
//	std::string baseDir = PHYSFS_getBaseDir();
//	std::string searchPath = *PHYSFS_getSearchPath();
//	std::string realDir = PHYSFS_getRealDir(path);
//
//	output.append(*PHYSFS_getSearchPath()).append("/");
//	output.append(PHYSFS_getRealDir(path)).append("/").append(path);
//}
//std::string FileSystem::GetPathRelativeToAssets(const char* originalPath) /*const*/
//{
//	std::string ret;
//	GetRealDir(originalPath, ret);
//
//	return ret;
//}

std::string FileSystem::NormalizePath(const char* full_path) /*const*/
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '\\')
			newPath[i] = '/';
	}
	return newPath;
}
std::string FileSystem::UnNormalizePath(const char* full_path) /*const*/
{
	std::string newPath(full_path);
	for (int i = 0; i < newPath.size(); ++i)
	{
		if (newPath[i] == '/')
			newPath[i] = '\\';
	}
	return newPath;
}

void FileSystem::SplitFilePath(const char* full_path, std::string* path, std::string* file, std::string* extension) /*const*/
{
	if (full_path != nullptr)
	{
		std::string full(full_path);
		size_t pos_separator = full.find_last_of("\\/");
		size_t pos_dot = full.find_last_of(".");

		if (path != nullptr)
		{
			if (pos_separator < full.length())
				* path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				* file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			else
				*file = full.substr(0, pos_dot);
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				* extension = full.substr(pos_dot + 1);
			else
				extension->clear();
		}
	}
}

unsigned int FileSystem::Load(const char* path, const char* file, char** buffer) /*const*/
{
	std::string full_path(path);
	full_path += file;
	return LoadToBuffer(full_path.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint FileSystem::LoadToBuffer(const char* file, char** buffer) /*const*/
{
	uint ret = 0;

	PHYSFS_file* fs_file = PHYSFS_openRead(file);

	if (fs_file != nullptr)
	{
		PHYSFS_sint64 size = PHYSFS_fileLength(fs_file);
		//LOG(LogType::L_ERROR, "[%s]", PHYSFS_getLastError())

		if (size > 0)
		{
			*buffer = new char[size + 1];
			uint readed = (uint)PHYSFS_read(fs_file, *buffer, 1, size);
			if (readed != size)
			{
				LOG(LogType::L_ERROR, "File System error while reading from file %s: %s\n", file, PHYSFS_getLastError());
				RELEASE_ARRAY(buffer);
			}
			else
			{
				ret = readed;
				//Adding end of file at the end of the buffer. Loading a shader file does not add this for some reason
				(*buffer)[size] = '\0';
			}
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG(LogType::L_ERROR, "File System error while closing file %s: %s\n", file, PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "File System error while opening file %s: %s\n", file, PHYSFS_getLastError());

	return ret;
}

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

/*Duplicate a file to a local PhysFS valid path*/
uint FileSystem::Copy(const char* file, const char* dir, std::string& outputFile)
{
	uint size = 0;

	if (file == nullptr || dir == nullptr)
	{
		/*assert(file != nullptr && dir != nullptr);*/
		return size;
	}

	std::FILE* filehandle;
	fopen_s(&filehandle, file, "rb");

	if (filehandle != nullptr)
	{
		fseek(filehandle, 0, SEEK_END);
		size = ftell(filehandle);
		rewind(filehandle);

		char* buffer = new char[size];
		size = fread(buffer, 1, size, filehandle);
		if (size > 0)
		{
			GetFileName(file, outputFile, true);
			outputFile.insert(0, "/");
			outputFile.insert(0, dir);

			size = Save(outputFile.data(), buffer, size, false);
			if (size > 0)
			{
				LOG(LogType::L_NORMAL, "FILE SYSTEM: Successfully copied file '%s' in dir '%s'", file, dir);
			}
			else
				LOG(LogType::L_ERROR, "FILE SYSTEM: Could not copy file '%s' in dir '%s'", file, dir);
		}
		else
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not read from file '%s'", file);

		RELEASE_ARRAY(buffer);
		fclose(filehandle);
	}
	else
		LOG(LogType::L_ERROR, "FILE SYSTEM: Could not open file '%s' to read", file);

	return size;
}


void FileSystem::CreateLibrarySoundBanks()
{
	std::string dir = "Assets/SoundBanks";
	std::vector<std::string> fileList;

	char** rc = PHYSFS_enumerateFiles(dir.c_str());
	char** iter;

	std::string directory = dir;

	for (iter = rc; *iter != nullptr; iter++)
	{
		if (!PHYSFS_isDirectory((directory + *iter).c_str()))
			fileList.push_back(*iter);
	}

	PHYSFS_freeList(rc);

	for (int i = 0; i < fileList.size(); i++)
	{
		uint found = fileList[i].find(".meta");
		if (found == std::string::npos)
		{
			std::string output;
			std::string fileDir = "Assets\\SoundBanks\\" + fileList[i];
			Copy(fileDir.c_str(), SOUNDS_PATH, output);
		}
	}
}



//std::string FileSystem::FileToText(const char* path)
//{
//	// Read from the text file
//	std::ifstream readFile(path);
//
//	std::stringstream buffer;
//	buffer << readFile.rdbuf();
//
//	// Close the file
//	readFile.close();
//
//	return buffer.str();
//}

void FileSystem::GetFileName(const char* file, std::string& fileName, bool extension)
{
	fileName = file;

	uint found = fileName.find_last_of("\\");
	if (found != std::string::npos)
		fileName = fileName.substr(found + 1, fileName.size());

	found = fileName.find_last_of("//");
	if (found != std::string::npos)
		fileName = fileName.substr(found + 1, fileName.size());

	if (!extension)
	{
		found = fileName.find_last_of(".");
		if (found != std::string::npos)
			fileName = fileName.substr(0, found);
	}
}

uint FileSystem::Save(const char* file, char* buffer, uint size, bool append)
{
	uint objCount = 0;

	std::string fileName;
	GetFileName(file, fileName, true);

	bool exists = Exists(file);

	PHYSFS_file* filehandle = nullptr;
	if (append)
		filehandle = PHYSFS_openAppend(file);
	else
		filehandle = PHYSFS_openWrite(file);

	if (filehandle != nullptr)
	{
		objCount = PHYSFS_writeBytes(filehandle, (const void*)buffer, size);

		if (objCount == size)
		{
			if (exists)
			{
				if (append)
				{
					LOG(LogType::L_NORMAL, "FILE SYSTEM: Append %u bytes to file '%s'", objCount, fileName.data());
				}
				else
					LOG(LogType::L_NORMAL, "FILE SYSTEM: File '%s' overwritten with %u bytes", fileName.data(), objCount);
			}
			else
				LOG(LogType::L_NORMAL, "FILE SYSTEM: New file '%s' created with %u bytes", fileName.data(), objCount);
		}
		else
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not write to file '%s'. ERROR: %s", fileName.data(), PHYSFS_getLastError());

		if (PHYSFS_close(filehandle) == 0)
			LOG(LogType::L_ERROR, "FILE SYSTEM: Could not close file '%s'. ERROR: %s", fileName.data(), PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "FILE SYSTEM: Could not open file '%s' to write. ERROR: %s", fileName.data(), PHYSFS_getLastError());

	return objCount;
}
