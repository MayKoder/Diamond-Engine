#include "MO_FileSystem.h"
#include "Globals.h"

#include "IM_MeshLoader.h"
#include "IM_FileSystem.h"

#include "Application.h"

#include "MO_Editor.h"
#include "MO_ResourceManager.h"
#include"MO_Scene.h"

#include "PhysFS/include/physfs.h"

//IMPORTANT TODO ASK: We should clear json usage, could be a hard hit on the grade
#include"parson/parson.h"
#include"DEJsonSupport.h"

//TODO: This should not be here
#pragma comment( lib, "DevIL/libx86/DevIL.lib" )
#include "DevIL\include\ilu.h"
#pragma comment( lib, "DevIL/libx86/ILU.lib" )
#include "DevIL\include\ilut.h"
#pragma comment( lib, "DevIL/libx86/ILUT.lib" )


M_FileSystem::M_FileSystem(Application* app, bool start_enabled) : Module(app, start_enabled)
{
}

M_FileSystem::~M_FileSystem()
{
}

bool M_FileSystem::Init()
{
	//Devil init
	LOG(LogType::L_NORMAL, "DevIL Init");
	ilInit();
	iluInit();
	ilutInit();
	ilutRenderer(ILUT_OPENGL);

	FileSystem::FSInit();
	MeshLoader::EnableDebugMode();

	return true;
}

bool M_FileSystem::Start()
{

	//TODO: Move to resource manager
	App->moduleResources->PopulateFileArray();
	GenerateAllMetaFiles();
	ImportAssetsToLibrary();

	//TODO: Should be updated kinda like assetsRoot;
	//TODO: Add Library/ to gitignore?
	GetAllFilesRecursive(App->moduleResources->meshesLibraryRoot);

	return true;
}

bool M_FileSystem::CleanUp()
{

	FileSystem::FSDeInit();
	MeshLoader::DisableDebugMode();

	return true;
}

void M_FileSystem::GetAllFiles(const char* directory, std::vector<AssetDir>& file_list)
{
	char** files = PHYSFS_enumerateFiles(directory);

	for (char** i = files; *i != nullptr; i++)
	{
		std::string str = std::string(directory) + std::string("/") + std::string(*i);
		std::string ext = "";
		FileSystem::SplitFilePath(str.c_str(), nullptr, nullptr, &ext);

		if (ext != "meta") 
		{
			if (FileSystem::IsDirectory(str.c_str()))
				file_list.push_back(AssetDir(*i, str.c_str(), GetLastModTime(str.c_str()), true)); //It's a folder
			else
				file_list.push_back(AssetDir(*i, str.c_str(), GetLastModTime(str.c_str()), false)); //It's a file, TODO: ADD FILE PATH
		}
	}

	PHYSFS_freeList(files);
}

void M_FileSystem::GetAllFilesRecursive(AssetDir& _file)
{
	GetAllFiles(_file.importPath.c_str(), _file.childDirs);

	if (_file.childDirs.size() != 0)
	{
		for (unsigned int i = 0; i < _file.childDirs.size(); i++)
		{
			if (_file.childDirs[i].isDir) 
			{
				//_file.childDirs[i].dirName.push_back('/');
				GetAllFilesRecursive(_file.childDirs[i]);
			}
		}
	}
}

uint64 M_FileSystem::GetLastModTime(const char* filename)
{
	PHYSFS_Stat stat;
	PHYSFS_stat(filename, &stat);
	return stat.modtime;
}

std::string M_FileSystem::OpenFileSelectDialog()
{
	char filename[MAX_PATH];
	filename[0] = '\0';

	OPENFILENAME ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = ".des\0*.des*\0"; //Text Files\0*.txt\0 //I DONT KNOW HOW TO FILTER .META FILES AAAAAAAH
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Select a File, yo!";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
	
	if (GetOpenFileNameA(&ofn))
		return std::string(filename);

	return std::string("\0");
}

std::string M_FileSystem::OpenSaveAsDialog()
{
	char filename[MAX_PATH];
	filename[0] = '\0';

	OPENFILENAMEA ofn;
	ZeroMemory(&filename, sizeof(filename));
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "Diamond Engine Scene (.des)\0"; //Text Files\0*.txt\0
	ofn.lpstrDefExt = "des";
	ofn.lpstrFile = filename;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrTitle = "Save file as";
	ofn.Flags = OFN_DONTADDTORECENT | OFN_NOCHANGEDIR;

	if (GetSaveFileNameA(&ofn))
		return std::string(filename);

	return std::string("\0");
}

void M_FileSystem::ToLocalAssetsPath(std::string& inp)
{
	std::string localPath = FileSystem::NormalizePath(inp.c_str());
	inp.clear();

	size_t pos = 0;
	pos = localPath.find(ASSETS_PATH);
	if (pos != std::string::npos)
	{
		localPath = localPath.substr(pos);
		if (localPath.c_str() != "") {
			inp = localPath;
			return;
		}
	}

	inp = localPath;
	localPath.clear();
	return;
}

int M_FileSystem::DeleteAssetFile(const char* fileDir)
{
	if (fileDir == nullptr)
		return 0;

	return PHYSFS_delete(fileDir);
}

void M_FileSystem::GenerateAllMetaFiles()
{
	App->moduleResources->assetsRoot.GenerateMetaRecursive();
}

void M_FileSystem::ImportAssetsToLibrary()
{
	App->moduleResources->assetsRoot.CreateLibraryFileRecursive();
}
