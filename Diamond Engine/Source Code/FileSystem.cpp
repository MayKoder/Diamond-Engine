#include "FileSystem.h"

#include "Globals.h"

#include "Application.h"

#include "ModuleRenderer3D.h"
#include "M_Scene.h"

#include <fstream>
#include <iostream>
#include <filesystem>

#include "PhysFS/include/physfs.h"
#include "MeshLoader.h"
#include"GameObject.h"
#include"C_Material.h"

#include"M_Editor.h"
#include"W_Inspector.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

std::string StringLogic::FileNameFromPath(const char* _path)
{
	std::string fileName(_path);

	fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.'));

	return fileName;
}

std::string StringLogic::GlobalToLocalPath(const char* _globalPath)
{
	std::string test = FileSystem::NormalizePath(_globalPath);

	size_t pos = 0;
	pos = test.find(ASSETS_PATH);
	if (pos != std::string::npos)
	{
		test = test.substr(pos);
		if (test.c_str() != "")
			return test;
	}

	test.clear();
	return test;
}

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

void FileSystem::LoadFile(const char* globalPath)
{
	std::string normalizedPath = NormalizePath(globalPath);
	std::string relativePath = StringLogic::GlobalToLocalPath(globalPath);

	//Duplicate file
	//BUG: This will only allow to work with files inside PhysFS dir
	std::string output = "";

	std::string fileName = StringLogic::GlobalToLocalPath(normalizedPath.c_str());
	if (fileName.length() == 0) {
		fileName = normalizedPath;
	}

	if (PHYSFS_exists(fileName.c_str()) == 0)
	{
		//TODO: Hardcoded directory
		Copy(globalPath, ASSETS_PATH, output);
		fileName = output;
	}

	//output = PHYSFS_getBaseDir();

	//Local dir?
	//std::string realDir = PHYSFS_getRealDir(fileName.c_str());
	//realDir += fileName;

	char* buffer = nullptr;
	uint size = FileSystem::Load(fileName.c_str(), &buffer);

	if (buffer != nullptr && size != 0) 
	{
		switch (GetTypeFromPath(globalPath))
		{

			case ImportType::NOTYPE:
				LOG(LogType::L_ERROR,  "File extension not supported yet");
				break;

			case ImportType::MESH: 
			{
				MeshLoader::ImportFBXFromBuffer(normalizedPath.c_str(), buffer, size, EngineExternal->moduleRenderer3D->globalMeshes, EngineExternal->moduleScene->root);

				break;
			}

			case ImportType::TEXTURE: 
			{
				GLuint textureID =MeshLoader::CustomLoadImage(buffer, size);
				EngineExternal->moduleRenderer3D->globalTextures.push_back(textureID);

				W_Inspector* inspector = dynamic_cast<W_Inspector*>(EngineExternal->moduleEditor->GetEditorWindow(EditorWindow::INSPECTOR));
				if (inspector && inspector->selectedGO) {
					C_Material* mat = dynamic_cast<C_Material*>(inspector->selectedGO->GetComponent(Component::Type::Material));
					if (mat) {
						mat->textureID = textureID;
					}
				}
				break;
			}

		}
	}

}

void FileSystem::CreateLibraryDirectories()
{
	CreateDir(LIBRARY_PATH);
	CreateDir(MESHES_PATH);
	CreateDir(TEXTURES_PATH);
	CreateDir(MATERIALS_PATH);
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

const char* FileSystem::GetWriteDir() /*const*/
{
	//TODO: erase first annoying dot (".")
	return PHYSFS_getWriteDir();
}

void FileSystem::DiscoverFiles(const char* directory, std::vector<std::string>& file_list, std::vector<std::string>& dir_list) /*const*/
{
	char** rc = PHYSFS_enumerateFiles(directory);
	char** i;

	for (i = rc; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		if (IsDirectory(str.c_str()))
			dir_list.push_back(*i);
		else
			file_list.push_back(*i);
	}

	PHYSFS_freeList(rc);
}

void FileSystem::GetAllFilesWithExtension(const char* directory, const char* extension, std::vector<std::string>& file_list) /*const*/
{
	std::vector<std::string> files;
	std::vector<std::string> dirs;
	DiscoverFiles(directory, files, dirs);

	for (uint i = 0; i < files.size(); i++)
	{
		std::string ext;
		SplitFilePath(files[i].c_str(), nullptr, nullptr, &ext);

		if (ext == extension)
			file_list.push_back(files[i]);
	}
}

void FileSystem::GetRealDir(const char* path, std::string& output) /*const*/
{
	output = PHYSFS_getBaseDir();

	std::string baseDir = PHYSFS_getBaseDir();
	std::string searchPath = *PHYSFS_getSearchPath();
	std::string realDir = PHYSFS_getRealDir(path);

	output.append(*PHYSFS_getSearchPath()).append("/");
	output.append(PHYSFS_getRealDir(path)).append("/").append(path);
}

std::string FileSystem::GetPathRelativeToAssets(const char* originalPath) /*const*/
{
	std::string ret;
	GetRealDir(originalPath, ret);

	return ret;
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
	return Load(full_path.c_str(), buffer);
}

// Read a whole file and put it in a new buffer
uint FileSystem::Load(const char* file, char** buffer) /*const*/
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
		//If it is a directory, we need to recursively remove all the files inside
		//if (IsDirectory(file))
		//{
		//	std::vector<std::string> containedFiles, containedDirs;
		//	PathNode rootDirectory = GetAllFiles(file);

		//	for (uint i = 0; i < rootDirectory.children.size(); ++i)
		//		Remove(rootDirectory.children[i].path.c_str());
		//}

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

uint64 FileSystem::GetLastModTime(const char* filename)
{
	return PHYSFS_getLastModTime(filename);
}

std::string FileSystem::GetUniqueName(const char* path, const char* name) /*const*/
{
	//TODO: modify to distinguix files and dirs?
	std::vector<std::string> files, dirs;
	DiscoverFiles(path, files, dirs);

	std::string finalName(name);
	bool unique = false;

	for (uint i = 0; i < 50 && unique == false; ++i)
	{
		unique = true;

		//Build the compare name (name_i)
		if (i > 0)
		{
			finalName = std::string(name).append("_");
			if (i < 10)
				finalName.append("0");
			finalName.append(std::to_string(i));
		}

		//Iterate through all the files to find a matching name
		for (uint f = 0; f < files.size(); ++f)
		{
			if (finalName == files[f])
			{
				unique = false;
				break;
			}
		}
	}
	return finalName;
}

// ------------ NEW STUFF ---------------- //

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
