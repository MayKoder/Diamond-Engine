#include "MeshLoader.h"

#include "Globals.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

void MeshLoader::EnableDebugMode()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	aiAttachLogStream(&stream);
}

void MeshLoader::DisableDebugMode()
{
	// detach log stream
	aiDetachAllLogStreams();
}

void MeshLoader::ImportFBX(const char* full_path, std::vector<Mesh*>& _meshes)
{
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiMesh* new_mesh = nullptr;
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			new_mesh = scene->mMeshes[i];
			LOG("%s", scene->mMeshes[i]->mName.C_Str());

			Mesh* _mesh = new Mesh();

			// copy vertices
			_mesh->vertices_count = new_mesh->mNumVertices;
			_mesh->vertices = new float[_mesh->vertices_count * 3];
			memcpy(_mesh->vertices, new_mesh->mVertices, sizeof(float) * _mesh->vertices_count * 3);
			LOG("New mesh with %d vertices", _mesh->vertices_count);


			// copy faces
			if (new_mesh->HasFaces())
			{
				_mesh->indices_count = new_mesh->mNumFaces * 3;
				_mesh->indices = new uint[_mesh->indices_count]; // assume each face is a triangle
				for (uint i = 0; i < new_mesh->mNumFaces; ++i)
				{
					if (new_mesh->mFaces[i].mNumIndices != 3) 
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else 
					{
						memcpy(&_mesh->indices[i * 3], new_mesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}
			_meshes.push_back(_mesh);
		}


		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene % s", full_path);
}
