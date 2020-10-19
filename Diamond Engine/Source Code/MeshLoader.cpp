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

			if (importedMesh->HasTextureCoords(0)) 
			{
				_mesh->texCoords_count = importedMesh->mNumVertices;
				_mesh->texCoords = new float[importedMesh->mNumVertices * 2];

				for (unsigned int i = 0; i < _mesh->texCoords_count; i++)
				{
					_mesh->texCoords[i * 2] = importedMesh->mTextureCoords[0][i].x;
					_mesh->texCoords[i * 2 + 1] = importedMesh->mTextureCoords[0][i].y;
				}
				_mesh->textureID = temporalTexID;

				LOG("New mesh with %d texture coords", _mesh->texCoords_count);
			}


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



