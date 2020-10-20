#include "MeshLoader.h"

#include "Globals.h"

#include "Application.h"

#include "M_Editor.h"
#include "ModuleRenderer3D.h"

#include "Mesh.h"

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

void MeshLoader::ImportFBX(const char* full_path, std::vector<Mesh*>& _meshes, int temporalTexID)
{
	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);

	if (scene != nullptr && scene->HasMeshes())
	{
		// Use scene->mNumMeshes to iterate on scene->mMeshes array
		aiMesh* importedMesh = nullptr;
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			importedMesh = scene->mMeshes[i];
			LOG("%s", scene->mMeshes[i]->mName.C_Str());

			Mesh* _mesh = new Mesh();

			// copy vertices
			_mesh->vertices_count = importedMesh->mNumVertices;
			_mesh->vertices = new float[_mesh->vertices_count * 3];

			memcpy(_mesh->vertices, importedMesh->mVertices, sizeof(float) * _mesh->vertices_count * 3);

			LOG("New mesh with %d vertices", _mesh->vertices_count);

			if (importedMesh->HasNormals())
			{
				_mesh->normals_count = _mesh->vertices_count;

				_mesh->normals = new float[_mesh->normals_count * 3];
				memcpy(_mesh->normals, importedMesh->mNormals, sizeof(float) * _mesh->normals_count * 3);

				LOG("New mesh with %d normals", _mesh->normals_count);
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
				_mesh->textureID = temporalTexID;

				LOG("New mesh with %d texture coords", _mesh->texCoords_count);
			}
			//if (importedMesh->HasVertexColors(0)) 
			//{

			//}


			// copy faces
			if (importedMesh->HasFaces())
			{
				_mesh->indices_count = importedMesh->mNumFaces * 3;
				_mesh->indices = new uint[_mesh->indices_count]; // assume each face is a triangle
				for (uint j = 0; j < importedMesh->mNumFaces; ++j)
				{
					if (importedMesh->mFaces[j].mNumIndices != 3) 
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else 
					{
						memcpy(&_mesh->indices[j * 3], importedMesh->mFaces[j].mIndices, 3 * sizeof(uint));
					}
				}
			}

			_mesh->GenBuffers();


			_mesh->generalWireframe = &EngineExternal->renderer3D->wireframe;
			_meshes.push_back(_mesh);
		}


		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene % s", full_path);
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



