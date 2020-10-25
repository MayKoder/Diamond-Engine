#include "MeshLoader.h"

#include "Globals.h"

#include "Application.h"

#include "M_Editor.h"
#include "ModuleRenderer3D.h"
#include "M_Scene.h"

#include "Mesh.h"
#include "GameObject.h"
#include "C_MeshRenderer.h"

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
		
		//Load all meshes into mesh vector
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			sceneMeshes.push_back(LoadMesh(scene->mMeshes[i]));
		}

		LOG(LogType::L_NORMAL, "-- Loading FBX as GameObject --");
		NodeToGameObject(sceneMeshes, scene->mRootNode, gmRoot, fileName.c_str());
		//aiMaterial* material = scene->mMaterials[importedMesh->mMaterialIndex];
		//uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

		//aiString path;
		//material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

		//EngineExternal->renderer3D->CustomLoadImage("Assets/BakerHouse/Baker_House.png");
		//path.Clear();

		//Only for memory cleanup, needs an update ASAP
		for (unsigned int i = 0; i < sceneMeshes.size(); i++)
		{
			EngineExternal->renderer3D->testMeshes.push_back(sceneMeshes[i]);
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
