#include "IM_MeshLoader.h"
#include "Application.h"
#include "MO_Editor.h"

#include "GameObject.h"
#include "C_Transform.h"
#include "C_MeshRenderer.h"
#include "C_Material.h"

#include "RE_Mesh.h"

#include "IM_FileSystem.h"
#include "IM_TextureImporter.h"
#include"IM_ModelImporter.h"

#include "MO_Scene.h"
#include "RE_Texture.h"

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

void MeshLoader::BufferToMeshes(const char* full_path, char* buffer, int size, GameObject* gmRoot)
{
	const aiScene* scene = aiImportFileFromMemory(buffer, size, aiProcessPreset_TargetRealtime_MaxQuality, nullptr);

	std::string fileName = StringLogic::FileNameFromPath(full_path);

	if (scene != nullptr && scene->HasMeshes())
	{

		std::vector<ResourceMesh*> sceneMeshes;
		std::vector<ResourceTexture*> testTextures;

		//This should not be here
		if (scene->HasMaterials())
		{
			//Needs to be moved to another place
			std::string generalPath(full_path);
			generalPath = generalPath.substr(0, generalPath.find_last_of("/\\") + 1);
			for (size_t k = 0; k < scene->mNumMaterials; k++)
			{
				aiMaterial* material = scene->mMaterials[k];
				aiString str = material->GetName();
				uint numTextures = material->GetTextureCount(aiTextureType_DIFFUSE);

				if (numTextures > 0) 
				{
					aiString path;
					material->GetTexture(aiTextureType_DIFFUSE, 0, &path);

					std::string textureFileName = FileSystem::NormalizePath(path.C_Str());
					size_t isLong = textureFileName.find_last_of("/");
					if(isLong != textureFileName.npos)
						textureFileName = textureFileName.substr(isLong + 1);

					std::string localPath = generalPath;
					localPath = localPath.substr(0, localPath.find_last_of('/')+1);
					localPath += FileSystem::NormalizePath(path.C_Str());

					char* buffer = nullptr;
					int size = FileSystem::LoadToBuffer(localPath.c_str(), &buffer);

					if (buffer != nullptr) 
					{
						int w = 0;
						int h = 0;

						GLuint id = TextureImporter::CustomLoadImage(buffer, size, &w, &h);
						TextureImporter::SaveDDS(buffer, size, textureFileName.substr(0, textureFileName.find_last_of(".")).c_str());

						//TODO: Request resource?
						//ResourceTexture* meshTexture = new ResourceTexture(id, w, h, textureFileName.c_str(), localPath.c_str());

						//testTextures.push_back(meshTexture);

						RELEASE_ARRAY(buffer)
					}

					path.Clear();
				}
				else 
				{
					//TODO: Temporal, think of a better way
					//TODO: Request resource?
					//ResourceTexture* meshTexture = new ResourceTexture(White);
					//testTextures.push_back(meshTexture);
				}
			}
		}

		//Load all meshes into mesh vector
		if (scene->HasMeshes()) 
		{
			for (unsigned int i = 0; i < scene->mNumMeshes; i++)
			{
				sceneMeshes.push_back(LoadMesh(scene->mMeshes[i]));
			}
		}

		//Save custom format model
		GameObject* root = new GameObject("First model GO", nullptr);
		NodeToGameObject(scene->mMeshes, testTextures, sceneMeshes, scene->mRootNode, root, fileName.c_str());
		ModelImporter::SaveModelCustom(root, fileName.c_str());
		delete root;

		//Only for memory cleanup, needs an update ASAP
		for (unsigned int i = 0; i < sceneMeshes.size(); i++)
		{
			delete sceneMeshes[i];
		}
		for (unsigned int i = 0; i < testTextures.size(); i++)
		{
			delete testTextures[i];
		}

		sceneMeshes.clear();
		testTextures.clear();

		aiReleaseImport(scene);

		ModelImporter::LoadModelCustom(fileName.c_str());
	}
	else
		LOG(LogType::L_ERROR, "Error loading scene % s", full_path);
}

//Following unity tree structure, comments represent blender tree structure
void MeshLoader::NodeToGameObject(aiMesh** meshArray, std::vector<ResourceTexture*>& sceneTextures, std::vector<ResourceMesh*>& _sceneMeshes, aiNode* node, GameObject* gmParent, const char* holderName)
{

	aiVector3D		aiTranslation, aiScale;
	aiQuaternion	aiRotation;

	//Decomposing transform matrix into translation rotation and scale
	node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);

	float3 pos(aiTranslation.x, aiTranslation.y, aiTranslation.z);
	float3 scale(aiScale.x, aiScale.y, aiScale.z);
	Quat rot(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);

	std::string nodeName = node->mName.C_Str();
	bool dummyFound = true;
	while (dummyFound)
	{
		dummyFound = false;

		//All dummy modules have one children (next dummy module or last module containing the mesh)
		if (nodeName.find("_$AssimpFbx$_") != std::string::npos && node->mNumChildren == 1)
		{
			//Dummy module have only one child node, so we use that one as our next node
			node = node->mChildren[0];

			// Accumulate transform 
			node->mTransformation.Decompose(aiScale, aiRotation, aiTranslation);
			pos += float3(aiTranslation.x, aiTranslation.y, aiTranslation.z);
			scale = float3(scale.x * aiScale.x, scale.y * aiScale.y, scale.z * aiScale.z);
			rot = rot * Quat(aiRotation.x, aiRotation.y, aiRotation.z, aiRotation.w);

			nodeName = node->mName.C_Str();
			dummyFound = true;
		}
	}




	for (unsigned int i = 0; i < node->mNumMeshes; i++)
	{
		ResourceMesh* meshPointer = _sceneMeshes[node->mMeshes[i]];

		GameObject* gmNode = new GameObject(node->mName.C_Str(), gmParent); //Name should be scene->mMeshes[node->mMeshes[i]]

		//Load mesh to GameObject
		C_MeshRenderer* gmMeshRenderer = dynamic_cast<C_MeshRenderer*>(gmNode->AddComponent(Component::Type::MeshRenderer));

		gmMeshRenderer->SetRenderMesh(meshPointer);

		aiMesh* importedMesh = meshArray[node->mMeshes[i]];
		if (importedMesh->mMaterialIndex < sceneTextures.size()) 
		{
			C_Material* material = dynamic_cast<C_Material*>(gmNode->AddComponent(Component::Type::Material));
			material->matTexture = sceneTextures[importedMesh->mMaterialIndex];
		}

		PopulateTransform(gmNode, pos, rot, scale);

		//if (transform->localTransform == transform->localTransform.nan || transform->globalTransform == float4x4::nan) {
		//	LOG(LogType::L_ERROR, "Some matrix loading is NAN");
		//}

		//gmParent->children.push_back(gmNode);
	}

	if (node->mNumChildren > 0)
	{
		GameObject* rootGO = gmParent;

		//WARNING: This could break the code if the gameobject that we are ignoring has some other components
		if (node->mNumChildren == 1 && node->mParent == nullptr && node->mChildren[0]->mNumChildren == 0) 
		{
			LOG(LogType::L_WARNING, "This is a 1 child gameObject, you could ignore the root node parent creation");
			node->mChildren[0]->mName = holderName;
		}
		else
		{
			rootGO = new GameObject(holderName, gmParent);
			PopulateTransform(rootGO, pos, rot, scale);
			//gmParent->children.push_back(rootGO);
		}

		
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(meshArray, sceneTextures, _sceneMeshes, node->mChildren[i], rootGO, node->mChildren[i]->mName.C_Str());
		}
	}

}

ResourceMesh* MeshLoader::LoadMesh(aiMesh* importedMesh)
{

	LOG(LogType::L_NORMAL, "%s", importedMesh->mName.C_Str());
	std::string file = MESHES_PATH;
	file += importedMesh->mName.C_Str();
	file += ".mmh";

	ResourceMesh* _mesh = new ResourceMesh(0);

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

	//TODO: Load vertex colors
	if (importedMesh->HasVertexColors(0)) 
	{
		LOG(LogType::L_ERROR, "ADD VERTEX COLORS");
	}


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


	//_mesh->GenBuffers();
	_mesh->generalWireframe = &EngineExternal->moduleRenderer3D->wireframe;

	//TODO: Save on own file format
	uint size = 0;
	char* buffer = (char*)_mesh->SaveCustomFormat(size);

	FileSystem::Save(file.c_str(), buffer, size, false);
	RELEASE_ARRAY(buffer);

	return _mesh;
}

void MeshLoader::PopulateTransform(GameObject* child, float3 position, Quat rotationQuat, float3 size)
{
	child->transform->SetTransformMatrix(position, rotationQuat, size);
}