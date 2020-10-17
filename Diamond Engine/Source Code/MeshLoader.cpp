#include "MeshLoader.h"

#include "Globals.h"

#include "Application.h"
#include "M_Editor.h"
#include "W_Console.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp.lib")

void MeshLoader::myCallback(const char* msg, char* userData) 
{
	if (Engine != nullptr && Engine->moduleEditor != nullptr)
	{
		W_Console* consoleWindow = dynamic_cast<W_Console*>(Engine->moduleEditor->GetEditorWindow(EditorWindow::CONSOLE));

		if (consoleWindow != nullptr)
			consoleWindow->AddLog(msg);
	}
}

void MeshLoader::EnableDebugMode()
{
	// Stream log messages to Debug window
	struct aiLogStream stream;
	stream = aiGetPredefinedLogStream(aiDefaultLogStream_DEBUGGER, nullptr);
	stream.callback = myCallback;

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
		aiMesh* importedMesh = nullptr;
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			importedMesh = scene->mMeshes[i];
			LOG("%s", scene->mMeshes[i]->mName.C_Str());

			Mesh* _mesh = new Mesh();

			// copy vertices
			_mesh->vertices_count = importedMesh->mNumVertices;
			_mesh->vertices = new float[_mesh->vertices_count * 3];

			//ASK: mVertices is a vector, can we memcpy a vector array into a float array?
			memcpy(_mesh->vertices, importedMesh->mVertices, sizeof(float) * _mesh->vertices_count * 3);
			LOG("New mesh with %d vertices", _mesh->vertices_count);

			if (importedMesh->HasNormals())
			{
				_mesh->normals_count = _mesh->vertices_count;

				_mesh->normals = new float[_mesh->normals_count * 3];
				memcpy(_mesh->normals, importedMesh->mNormals, sizeof(float) * _mesh->normals_count * 3);

				//for (int i = 0; i < _mesh->normals_count * 3; i += 3)
				//{
				//	LOG("Normal = %f, %f, %f", _mesh->normals[i], _mesh->normals[i + 1], _mesh->normals[i + 2]);
				//}

				LOG("New mesh with %d normals", _mesh->normals_count);
			}


			// copy faces
			if (importedMesh->HasFaces())
			{
				_mesh->indices_count = importedMesh->mNumFaces * 3;
				_mesh->indices = new uint[_mesh->indices_count]; // assume each face is a triangle
				for (uint i = 0; i < importedMesh->mNumFaces; ++i)
				{
					if (importedMesh->mFaces[i].mNumIndices != 3) 
					{
						LOG("WARNING, geometry face with != 3 indices!");
					}
					else 
					{
						memcpy(&_mesh->indices[i * 3], importedMesh->mFaces[i].mIndices, 3 * sizeof(uint));
					}
				}
			}

			_mesh->GenBuffers();
			_meshes.push_back(_mesh);
		}


		aiReleaseImport(scene);
	}
	else
		LOG("Error loading scene % s", full_path);
}
