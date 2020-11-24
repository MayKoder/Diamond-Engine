#include "I_FileSystem.h"
#include"I_TextureImporter.h"

#include "Globals.h"

#include "Application.h"
#include "M_Scene.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include "PhysFS/include/physfs.h"
#include "I_MeshLoader.h"
#include"GameObject.h"
#include"C_Material.h"

#include"M_Editor.h"
#include"Texture.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

/*Get file name from any path*/
std::string StringLogic::FileNameFromPath(const char* _path)
{
	std::string fileName(_path);

	fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.'));

	return fileName;
}

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
	FileSystem::CreateLibraryDirectories();
}

void FileSystem::FSDeInit()
{
	PHYSFS_deinit();
}

/*Load any file with a global path*/
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

	if (PHYSFS_exists(fileNameAndExtension.c_str()) == 0)
	{
		Copy(globalPath, ASSETS_PATH, output);
		fileNameAndExtension = output;
	}

	//TODO: Load new file to buffer
	//Convert to custom file format
	//Save custom file

	char* buffer = nullptr;
	uint size = FileSystem::LoadToBuffer(fileNameAndExtension.c_str(), &buffer);

	if (buffer != nullptr && size != 0) 
	{
		switch (iType)
		{

			case ImportType::MESH: 
			{
				//MeshLoader::ImportFBXFromBuffer(normalizedPath.c_str(), buffer, size, EngineExternal->moduleScene->root);
				MeshLoader::BufferToMeshes(normalizedPath.c_str(), buffer, size, EngineExternal->moduleScene->root);
				break;
			}

			case ImportType::TEXTURE: 
			{
				int w = 0; int h = 0;
				GLuint id = TextureImporter::CustomLoadImage(buffer, size, &w, &h);

				std::string fileName;
				GetFileName(fileNameAndExtension.c_str(), fileName, false);
				TextureImporter::SaveDDS(buffer, size, fileName.c_str());

				Texture* material = new Texture(id, w, h, fileNameAndExtension.substr(fileNameAndExtension.find_last_of('/') + 1), fileNameAndExtension);
				EngineExternal->moduleRenderer3D->globalTextures.push_back(material);

				if (EngineExternal->moduleEditor->GetSelectedGO()) 
				{
					C_Material* mat = dynamic_cast<C_Material*>(EngineExternal->moduleEditor->GetSelectedGO()->GetComponent(Component::Type::Material));
					if (mat) 
					{
						mat->matTexture = material;
					}
					else {
						C_Material* mat = dynamic_cast<C_Material*>(EngineExternal->moduleEditor->GetSelectedGO()->AddComponent(Component::Type::Material));
						mat->matTexture = material;

					}
				}
				break;
			}

		}
		RELEASE_ARRAY(buffer);
	}

}

void FileSystem::CreateLibraryDirectories()
{
	CreateDir(LIBRARY_PATH);
	CreateDir(MESHES_PATH);
	//CreateDir(TEXTURES_PATH);
	CreateDir(MATERIALS_PATH);
	CreateDir(SCENES_PATH);
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
				*path = full.substr(0, pos_separator + 1);
			else
				path->clear();
		}

		if (file != nullptr)
		{
			if (pos_separator < full.length())
				*file = full.substr(pos_separator + 1, pos_dot - pos_separator - 1);
			else
				*file = full.substr(0, pos_dot);
		}

		if (extension != nullptr)
		{
			if (pos_dot < full.length())
				*extension = full.substr(pos_dot + 1);
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

bool FileSystem::Remove(const char* file)
{
	bool ret = false;

	if (file != nullptr)
	{
		if (PHYSFS_delete(file) != 0)
		{
			LOG(LogType::L_NORMAL, "File deleted: [%s]", file);
			ret = true;
		}
		else
			LOG(LogType::L_ERROR, "File System error while trying to delete [%s]: %s", file, PHYSFS_getLastError());
	}

	return ret;
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
