#include "M_FileSystem.h"
#include "Globals.h"

#include "I_MeshLoader.h"
#include "I_FileSystem.h"

#include "Application.h"

#include "M_Editor.h"
#include "W_Assets.h"

#include "PhysFS/include/physfs.h"

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
	dynamic_cast<W_Assets*>(App->moduleEditor->GetEditorWindow(EditorWindow::ASSETS))->PopulateFileArray();
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
		if (FileSystem::IsDirectory(str.c_str()))
			file_list.push_back(AssetDir(*i, str.c_str(), true)); //It's a folder
		else
			file_list.push_back(AssetDir(*i, str.c_str(), false)); //It's a file, TODO: ADD FILE PATH
	}

	PHYSFS_freeList(files);
}

void M_FileSystem::GetAllFilesRecursive(AssetDir& _file)
{
	//if (!Exists(directory))
	//	return;
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

AssetDir::AssetDir(const char* _dName, const char* _imPath, bool _dir) : isDir(_dir)
{
	dirName = _dName;
	dirName.push_back('\0');
	importPath = _imPath;
	importPath.push_back('\0');
}

AssetDir::~AssetDir()
{
	dirName.clear();
	childDirs.clear();
	importPath.clear();
}

void AssetDir::ClearData()
{
	dirName.clear();
	childDirs.clear();
	isDir = false;
	importPath.clear();
}
