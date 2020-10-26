#include "FileSystem.h"

#include "Globals.h"

#include "Application.h"

#include "M_Editor.h"
#include "ModuleRenderer3D.h"
#include "M_Scene.h"

#include "Mesh.h"
#include "GameObject.h"
#include "C_MeshRenderer.h"
#include "C_Transform.h"

#include"MathGeoLib/include/Math/Quat.h"

#ifdef _DEBUG
#pragma comment (lib, "MathGeoLib/libx86/Debug/MathGeoLib.lib")
#else
#pragma comment (lib, "MathGeoLib/libx86/Release/MathGeoLib.lib")
#endif

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")


void  MeshLoader::logCallback(const char* message, char* user)
{
	EngineExternal->moduleEditor->LogToConsole(message);
}

void MeshLoader::EnableDebugMode()
{
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = logCallback;
	aiAttachLogStream(&stream);
}

void MeshLoader::DisableDebugMode()
{
	// detach log stream
	aiDetachAllLogStreams();
}

void MeshLoader::ImportFBX(const char* full_path, std::vector<Mesh*>& _meshes, GameObject* gmRoot)
{
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	std::string fileName = StringLogic::FileNameFromPath(full_path);
	
	if (scene != nullptr && scene->HasMeshes())
	{

		//BUG: This is a problem, meshes are shared, but should also be deleted, we are fucked
		std::vector<Mesh*> sceneMeshes;
		std::vector<GLuint*> sceneTextures;
		
		//Load all meshes into mesh vector
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			sceneMeshes.push_back(LoadMesh(scene->mMeshes[i]));
		}


		LOG(LogType::L_NORMAL, "-- Loading FBX as GameObject --");
		NodeToGameObject(sceneMeshes, scene->mRootNode, gmRoot, fileName.c_str());


		//Only for memory cleanup, needs an update ASAP
		for (unsigned int i = 0; i < sceneMeshes.size(); i++)
		{
			EngineExternal->renderer3D->globalMeshes.push_back(sceneMeshes[i]);
		}

		//This should not be here
		if (scene->HasMaterials()) 
		{
			//Needs to be moved to another place
			std::string generalPath(full_path);
			generalPath = generalPath.substr(0, generalPath.find_last_of("/\\") + 1);
			for (size_t k = 0; k < scene->mNumMaterials; k++)
			{
				aiMaterial* material = scene->mMaterials[k];
				uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

				aiString path;
				material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
				generalPath += path.C_Str();

				EngineExternal->renderer3D->CustomLoadImage(generalPath.c_str());
				path.Clear();
			}
		}

		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene % s", full_path);
}

void MeshLoader::NodeToGameObject(std::vector<Mesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName)
{
	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* meshPointer = _sceneMeshes[node->mMeshes[i]];

		GameObject* gmNode = new GameObject(node->mName.C_Str());
		//Load mesh to GameObject
		C_MeshRenderer* gmMeshRenderer = dynamic_cast<C_MeshRenderer*>(gmNode->AddComponent(Component::Type::MeshRenderer));
		gmMeshRenderer->_mesh = meshPointer;

		C_Transform* transform = dynamic_cast<C_Transform*>(gmNode->GetComponent(Component::Type::Transform));
		
		aiVector3D T, S;
		aiQuaternion R;
		node->mTransformation.Decompose(S, R, T);

		float3 position(T.x, T.y, T.z);
		float3 size(S.x, S.y, S.z);
		Quat rotationQuat(R.x, R.y, R.z, R.w);
		//LOG(LogType::L_WARNING, "%f, %f, %f", position.x, position.y, position.z);
		//LOG(LogType::L_WARNING, "%f, %f, %f", size.x, size.y, size.z);
		//LOG(LogType::L_WARNING, "%f, %f, %f, %f", rotationQuat.x, rotationQuat.y, rotationQuat.z, rotationQuat.w);
		transform->localTransform = float4x4::FromTRS(position, rotationQuat, size);
		transform->globalTransform = dynamic_cast<C_Transform*>(gmParent->GetComponent(Component::Type::Transform))->globalTransform * transform->localTransform;

		//if (transform->localTransform == transform->localTransform.nan || transform->globalTransform == float4x4::nan) {
		//	LOG(LogType::L_ERROR, "Some matrix loading is NAN");
		//}

		gmParent->children.push_back(gmNode);
	}

	if (node->mNumChildren > 0) 
	{
		GameObject* childrenHolder = new GameObject(holderName);
		gmParent->children.push_back(childrenHolder);

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(_sceneMeshes, node->mChildren[i], childrenHolder, node->mName.C_Str());
		}
	}

}

Mesh* MeshLoader::LoadMesh(aiMesh* importedMesh)
{

	LOG(LogType::L_NORMAL, "%s", importedMesh->mName.C_Str());
	Mesh* _mesh = new Mesh();

	// copy vertices
	_mesh->vertices_count = importedMesh->mNumVertices;
	_mesh->vertices = new float[_mesh->vertices_count * 3];

	memcpy(_mesh->vertices, importedMesh->mVertices, sizeof(float) * _mesh->vertices_count * 3);

	LOG(LogType::L_NORMAL, "New mesh with %d vertices", _mesh->vertices_count);

	if (importedMesh->HasNormals())
	{
		_mesh->normals_count = _mesh->vertices_count;

		_mesh->normals = new float[_mesh->normals_count * 3];
		memcpy(_mesh->normals, importedMesh->mNormals, sizeof(float) * _mesh->normals_count * 3);

		LOG(LogType::L_NORMAL, "New mesh with %d normals", _mesh->normals_count);
	}

	//So are we really only supporting 1 channel uv and colors?
	if (importedMesh->HasTextureCoords(0))
	{
		_mesh->texCoords_count = importedMesh->mNumVertices;
		_mesh->texCoords = new float[importedMesh->mNumVertices * 2];

		for (unsigned int k = 0; k < _mesh->texCoords_count; k++)
		{
			_mesh->texCoords[k * 2] = importedMesh->mTextureCoords[0][k].x;
			_mesh->texCoords[k * 2 + 1] = importedMesh->mTextureCoords[0][k].y;
		}
		//_mesh->textureID = temporalTexID;

		LOG(LogType::L_NORMAL, "New mesh with %d texture coords", _mesh->texCoords_count);
	}
	//if (importedMesh->HasVertexColors(0)) 
	//{

	//}


	// Generate indices
	if (importedMesh->HasFaces())
	{
		_mesh->indices_count = importedMesh->mNumFaces * 3;
		_mesh->indices = new uint[_mesh->indices_count]; // assume each face is a triangle
		for (uint j = 0; j < importedMesh->mNumFaces; ++j)
		{
			if (importedMesh->mFaces[j].mNumIndices != 3)
			{
				LOG(LogType::L_WARNING, "WARNING, geometry face with != 3 indices!");
			}
			else
			{
				memcpy(&_mesh->indices[j * 3], importedMesh->mFaces[j].mIndices, 3 * sizeof(uint));
			}
		}
	}


	_mesh->GenBuffers();
	_mesh->generalWireframe = &EngineExternal->renderer3D->wireframe;

	return _mesh;
}

ImportType MeshLoader::GetTypeFromPath(const char* path)
{
	std::string ext(path);
	ext = ext.substr(ext.find_last_of('.') + 1);

	for (int i = 0; i < ext.length(); i++)
	{
		ext[i] = std::tolower(ext[i]);
	}

	if (ext == "fbx")
		return ImportType::MESH;
	if (ext == "tga" || ext == "png" || ext == "jpg")
		return ImportType::TEXTURE;

	return ImportType::NOTYPE;
}

//void MeshLoader::GenerateCheckerTexture()
//{
//	GLubyte checkerImage[64][64][4];
//	for (int i = 0; i < 64; i++) {
//		for (int j = 0; j < 64; j++) {
//			int c = ((((i & 0x8) == 0) ^ (((j & 0x8)) == 0))) * 255;
//			checkerImage[i][j][0] = (GLubyte)c;
//			checkerImage[i][j][1] = (GLubyte)c;
//			checkerImage[i][j][2] = (GLubyte)c;
//			checkerImage[i][j][3] = (GLubyte)255;
//		}
//	}
//}

std::string StringLogic::FileNameFromPath(const char* _path)
{
	std::string fileName(_path);

	fileName = fileName.substr(fileName.find_last_of("/\\") + 1);
	fileName = fileName.substr(0, fileName.find_last_of('.'));

	return fileName;
}

//--------------------- TODO: PHYSFS MARC VERSION, MAKE YOUR OWN -----------------------//
//--------------------- TODO: PHYSFS MARC VERSION, MAKE YOUR OWN -----------------------//
//--------------------- TODO: PHYSFS MARC VERSION, MAKE YOUR OWN -----------------------//
#include <fstream>
#include <filesystem>
#include "PhysFS/include/physfs.h"

#pragma comment( lib, "PhysFS/libx86/physfs.lib" )

void MeshLoader::FSInit()
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
	FileSystem::CreateLibraryDirectories();
}

void MeshLoader::FSDeInit()
{
	PHYSFS_deinit();
}

void FileSystem::LoadFile(const char* globalPath)
{
	std::string normalizedPath = NormalizePath(globalPath);
	std::string finalPath;

	switch (MeshLoader::GetTypeFromPath(globalPath))
	{

		case ImportType::NOTYPE:
			LOG(LogType::L_ERROR,  "File extension not supported yet");
			break;

		case ImportType::MESH:
			//Duplicate file
			DuplicateFile(normalizedPath.c_str(), "Assets", finalPath);
			//Load file data
			break;

		case ImportType::TEXTURE:
			//Duplicate file
			DuplicateFile(normalizedPath.c_str(), "Assets", finalPath);
			//Load file data
			break;

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

bool FileSystem::HasExtension(const char* path) /*const*/
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext != "";
}

bool FileSystem::HasExtension(const char* path, std::string extension) /*const*/
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	return ext == extension;
}

bool FileSystem::HasExtension(const char* path, std::vector<std::string> extensions) /*const*/
{
	std::string ext = "";
	SplitFilePath(path, nullptr, nullptr, &ext);
	if (ext == "")
		return true;
	for (uint i = 0; i < extensions.size(); i++)
	{
		if (extensions[i] == ext)
			return true;
	}
	return false;
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
		PHYSFS_sint32 size = (PHYSFS_sint32)PHYSFS_fileLength(fs_file);

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

bool FileSystem::DuplicateFile(const char* file, const char* dstFolder, std::string& relativePath)
{
	std::string fileStr, extensionStr;
	SplitFilePath(file, nullptr, &fileStr, &extensionStr);

	relativePath = relativePath.append(dstFolder).append("/") + fileStr.append(".") + extensionStr;
	std::string finalPath = std::string(*PHYSFS_getSearchPath()).append("/") + relativePath;

	return DuplicateFile(file, finalPath.c_str());

}

bool FileSystem::DuplicateFile(const char* srcFile, const char* dstFile)
{
	//TODO: Compare performance to calling Load(srcFile) and then Save(dstFile)
	std::ifstream src;
	src.open(srcFile, std::ios::binary);
	bool srcOpen = src.is_open();
	std::ofstream  dst(dstFile, std::ios::binary);
	bool dstOpen = dst.is_open();

	dst << src.rdbuf();

	src.close();
	dst.close();

	if (srcOpen && dstOpen)
	{
		LOG(LogType::L_NORMAL, "File Duplicated Correctly");
		return true;
	}
	else
	{
		LOG(LogType::L_ERROR, "File could not be duplicated");
		return false;
	}
}

int close_sdl_rwops(SDL_RWops* rw)
{
	RELEASE_ARRAY(rw->hidden.mem.base);
	SDL_FreeRW(rw);
	return 0;
}

// Save a whole buffer to disk
uint FileSystem::Save(const char* file, const void* buffer, unsigned int size, bool append) /*const*/
{
	unsigned int ret = 0;

	bool overwrite = PHYSFS_exists(file) != 0;
	PHYSFS_file* fs_file = (append) ? PHYSFS_openAppend(file) : PHYSFS_openWrite(file);

	if (fs_file != nullptr)
	{
		uint written = (uint)PHYSFS_write(fs_file, (const void*)buffer, 1, size);
		if (written != size)
		{
			LOG(LogType::L_ERROR, "[error] File System error while writing to file %s: %s", file, PHYSFS_getLastError());
		}
		else
		{
			//if (append == true)
			//	LOG(LogType::L_NORMAL, "Added %u data to [%s%s]", size, GetWriteDir(), file);
			//else if (overwrite == true)
			//	LOG(LogType::L_NORMAL, "File [%s%s] overwritten with %u bytes", GetWriteDir(), file, size);
			//else
			//	LOG(LogType::L_NORMAL, "New file created [%s%s] of %u bytes", GetWriteDir(), file, size);

			//ret = written;
		}

		if (PHYSFS_close(fs_file) == 0)
			LOG(LogType::L_ERROR, "[error] File System error while closing file %s: %s", file, PHYSFS_getLastError());
	}
	else
		LOG(LogType::L_ERROR, "[error] File System error while opening file %s: %s", file, PHYSFS_getLastError());

	return ret;
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
