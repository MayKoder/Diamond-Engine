#include "MeshLoader.h"
#include "Application.h"
#include "M_Editor.h"

#include "GameObject.h"
#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Material.h"

#include "Mesh.h"

#include "FileSystem.h"
#include "M_Scene.h"
#include "Texture.h"

#include"MathGeoLib/include/Math/Quat.h"

#include "DevIL\include\ilu.h"
#include "DevIL\include\ilut.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc142-mt.lib")


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

//void MeshLoader::ImportFBX(const char* full_path, std::vector<Mesh*>& _meshes, GameObject* gmRoot)
//{
//	const aiScene* scene = aiImportFile(full_path, aiProcessPreset_TargetRealtime_MaxQuality);
//
//	std::string fileName = StringLogic::FileNameFromPath(full_path);
//
//	if (scene != nullptr && scene->HasMeshes())
//	{
//
//		//BUG: This is a problem, meshes are shared, but should also be deleted, we are fucked
//		std::vector<Mesh*> sceneMeshes;
//		std::vector<GLuint*> sceneTextures;
//
//		//Load all meshes into mesh vector
//		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
//		{
//			sceneMeshes.push_back(LoadMesh(scene->mMeshes[i]));
//		}
//
//
//		LOG(LogType::L_NORMAL, "-- Loading FBX as GameObject --");
//		NodeToGameObject(scene->mMeshes, sceneMeshes, scene->mRootNode, gmRoot, fileName.c_str());
//
//
//		//Only for memory cleanup, needs an update ASAP
//		for (unsigned int i = 0; i < sceneMeshes.size(); i++)
//		{
//			EngineExternal->renderer3D->globalMeshes.push_back(sceneMeshes[i]);
//		}
//
//		//This should not be here
//		if (scene->HasMaterials())
//		{
//			//Needs to be moved to another place
//			std::string generalPath(full_path);
//			generalPath = generalPath.substr(0, generalPath.find_last_of("/\\") + 1);
//			for (size_t k = 0; k < scene->mNumMaterials; k++)
//			{
//				aiMaterial* material = scene->mMaterials[k];
//				uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);
//
//				aiString path;
//				material->GetTexture(aiTextureType_DIFFUSE, 0, &path);
//				generalPath += path.C_Str();
//
//				EngineExternal->renderer3D->CustomLoadImage(generalPath.c_str());
//				path.Clear();
//			}
//		}
//
//		aiReleaseImport(scene);
//	}
//	else
//		LOG(LogType::L_ERROR, "Error loading scene % s", full_path);
//}

void MeshLoader::ImportFBXFromBuffer(const char* full_path, char* buffer, int size, GameObject* gmRoot)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	//aiVector3t<float> factor;
	//scene->mMetaData->Get("OriginalUpAxis", factor);
	//
	//for (size_t i = 0; i < scene->mMetaData->mNumProperties; i++)
	//{
	//	const char* name;
	//	name = scene->mMetaData->mKeys[i].C_Str();
	//	LOG(LogType::L_NORMAL, "%s", name);
	//}

	std::string fileName = StringLogic::FileNameFromPath(full_path);

	if (scene != nullptr && scene->HasMeshes())
	{

		//BUG: This is a problem, meshes are shared, but should also be deleted, we are fucked
		std::vector<Mesh*> sceneMeshes;
		//std::vector<GLuint> sceneTextures;
		std::vector<Texture*> testTextures;

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

				if (numTextures > 0) 
				{
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

					std::string localPath = StringLogic::GlobalToLocalPath(full_path);
					localPath = localPath.substr(0, localPath.find_last_of('/')+1);
					localPath += FileSystem::NormalizePath(path.C_Str());

					char* buffer = nullptr;
					int size = FileSystem::Load(localPath.c_str(), &buffer);

					if (buffer != nullptr) 
					{
						int w = 0;
						int h = 0;

						GLuint id = CustomLoadImage(buffer, size, &w, &h);
						Texture* Test = new Texture(id, w, h);
						testTextures.push_back(Test);

						RELEASE_ARRAY(buffer)
					}

					path.Clear();
				}
			}
		}

		//Load all meshes into mesh vector
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			sceneMeshes.push_back(LoadMesh(scene->mMeshes[i]));
		}
		


		LOG(LogType::L_NORMAL, "-- Loading FBX as GameObject --");
		NodeToGameObject(scene->mMeshes, testTextures, sceneMeshes, scene->mRootNode, gmRoot, fileName.c_str());


		//Only for memory cleanup, needs an update ASAP
		for (unsigned int i = 0; i < sceneMeshes.size(); i++)
		{
			EngineExternal->moduleRenderer3D->globalMeshes.push_back(sceneMeshes[i]);
		}
		for (unsigned int i = 0; i < testTextures.size(); i++)
		{
			EngineExternal->moduleRenderer3D->globalTextures.push_back(testTextures[i]);
		}

		sceneMeshes.clear();
		testTextures.clear();

		aiReleaseImport(scene);
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene % s", full_path);
}

//Following unity tree structure, comments represent blender tree structure
void MeshLoader::NodeToGameObject(aiMesh** meshArray, std::vector<Texture*>& sceneTextures, std::vector<Mesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName)
{
	//GameObject* rootGO = new GameObject(node->mName.C_Str());
	//rootGO->parent = gmParent;
	//gmParent->children.push_back(rootGO);

	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		Mesh* meshPointer = _sceneMeshes[node->mMeshes[i]];

		GameObject* gmNode = new GameObject(node->mName.C_Str()); //Name should be scene->mMeshes[node->mMeshes[i]]
		gmNode->parent = gmParent;

		//Load mesh to GameObject
		C_MeshRenderer* gmMeshRenderer = dynamic_cast<C_MeshRenderer*>(gmNode->AddComponent(Component::Type::MeshRenderer));

		gmMeshRenderer->_mesh = meshPointer;

		aiMesh* importedMesh = meshArray[node->mMeshes[i]];
		if (importedMesh->mMaterialIndex < sceneTextures.size()) 
		{
			C_Material* material = dynamic_cast<C_Material*>(gmNode->AddComponent(Component::Type::Material));
			material->matTexture = sceneTextures[importedMesh->mMaterialIndex];
		}

		PopulateTransform(gmNode, node);

		//if (transform->localTransform == transform->localTransform.nan || transform->globalTransform == float4x4::nan) {
		//	LOG(LogType::L_ERROR, "Some matrix loading is NAN");
		//}

		gmParent->children.push_back(gmNode);
	}

	if (node->mNumChildren > 0)
	{
		GameObject* rootGO = gmParent;

		if (node->mNumChildren == 1 && node->mParent == nullptr && node->mChildren[0]->mNumChildren == 0) 
		{
			LOG(LogType::L_WARNING, "This is a 1 child gameObject, you could ignore the root node parent creation");
			node->mChildren[0]->mName = holderName;
		}
		else
		{
			rootGO = new GameObject(holderName);
			rootGO->parent = gmParent;
			PopulateTransform(rootGO, node);
			gmParent->children.push_back(rootGO);
		}

		
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(meshArray, sceneTextures, _sceneMeshes, node->mChildren[i], rootGO, node->mChildren[i]->mName.C_Str());
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
	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;

	return _mesh;
}

void MeshLoader::PopulateTransform(GameObject* child, aiNode* node)
{

	C_Transform* transform = child->transform;
	C_Transform* parentTransform = child->parent->transform;

	aiVector3D T, S;
	aiQuaternion R;
	node->mTransformation.Decompose(S, R, T);

	float3 position(T.x, T.y, T.z);
	float3 size(S.x /** (1 / S.x)*/, S.y /** (1 / S.y)*/, S.z /** (1 / S.z)*/);
	Quat rotationQuat(R.x, R.y, R.z, R.w);

	transform->SetTransformMatrix(position, rotationQuat, size, parentTransform);

	//transform->localTransform = float4x4::FromTRS(position, rotationQuat, size);
	//transform->globalTransform = parentTransform->globalTransform * transform->localTransform;
}

GLuint MeshLoader::CustomLoadImage(char* buffer, int size, int* w, int* h)
{
	ILuint imageID;
	ilGenImages(1, &imageID);
	ilBindImage(imageID);

	if (!ilLoadL(IL_TYPE_UNKNOWN, buffer, size))
	{
		LOG(LogType::L_ERROR, "Image not loaded");
	}


	if (w)
		*w = ilGetInteger(IL_IMAGE_WIDTH);
	if(h)
		*h = ilGetInteger(IL_IMAGE_HEIGHT);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	GLuint glID = ilutGLBindTexImage();

	glBindTexture(GL_TEXTURE_2D, glID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);




	ilDeleteImages(1, &imageID);
	glBindTexture(GL_TEXTURE_2D, 0);

	return glID;
}
