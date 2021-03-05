#include "IM_MeshLoader.h"
#include "Application.h"
#include "MO_Editor.h"

#include "GameObject.h"
#include "CO_Transform.h"
#include "CO_MeshRenderer.h"
#include "CO_Material.h"

#include "RE_Mesh.h"
#include "RE_Texture.h"

#include "IM_FileSystem.h"
#include "IM_TextureImporter.h"
#include "IM_ModelImporter.h"

#include "MO_Scene.h"
#include "MO_ResourceManager.h"

#include "Assimp/include/cimport.h"
#include "Assimp/include/scene.h"
#include "Assimp/include/postprocess.h"
#include "Assimp/include/cfileio.h"

#pragma comment (lib, "Assimp/libx86/assimp-vc142-mt.lib")

#ifndef STANDALONE
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
#endif // !STANDALONE


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
		C_MeshRenderer* gmMeshRenderer = dynamic_cast<C_MeshRenderer*>(gmNode->AddComponent(Component::TYPE::MESH_RENDERER));

		gmMeshRenderer->SetRenderMesh(meshPointer);

		aiMesh* importedMesh = meshArray[node->mMeshes[i]];
		if (importedMesh->mMaterialIndex < sceneTextures.size() && sceneTextures[importedMesh->mMaterialIndex] != nullptr)
		{
			C_Material* material = dynamic_cast<C_Material*>(gmNode->AddComponent(Component::TYPE::MATERIAL));
			material->matTexture = sceneTextures[importedMesh->mMaterialIndex];
		}

		PopulateTransform(gmNode, pos, rot, scale);
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
			std::string finalString = std::string(holderName);
			size_t index = finalString.find("_$AssimpFbx$_");
			if (index != std::string::npos) {
				finalString = finalString.erase(index, std::string::npos);
				holderName = finalString.c_str();
			}
			rootGO = new GameObject(holderName, gmParent);
			PopulateTransform(rootGO, pos, rot, scale);
		}

		
		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			NodeToGameObject(meshArray, sceneTextures, _sceneMeshes, node->mChildren[i], rootGO, node->mChildren[i]->mName.C_Str());
		}
	}
	else if (node->mNumMeshes == 0)
	{
		std::string finalString = std::string(holderName);
		size_t index = finalString.find("_$AssimpFbx$_");
		if (index != std::string::npos) {
			finalString = finalString.erase(index, std::string::npos);
			holderName = finalString.c_str();
		}
		GameObject* noChildrenGO = new GameObject(holderName, gmParent);
		PopulateTransform(noChildrenGO, pos, rot, scale);
	}

}

ResourceMesh* MeshLoader::LoadMesh(aiMesh* importedMesh, uint oldUID)
{
	uint UID = oldUID;
	if (UID == 0)
		UID = EngineExternal->moduleResources->GenerateNewUID();

	LOG(LogType::L_NORMAL, "%s", importedMesh->mName.C_Str());
	std::string file = MESHES_PATH;
	file += std::to_string(UID);
	file += ".mmh";

	ResourceMesh* _mesh = dynamic_cast<ResourceMesh*>(EngineExternal->moduleResources->CreateNewResource("", UID, Resource::Type::MESH));

	// copy vertices
	_mesh->vertices_count = importedMesh->mNumVertices;
	_mesh->vertices = new float[_mesh->vertices_count * VERTEX_ATTRIBUTES];

	for (size_t i = 0; i < _mesh->vertices_count; i++)
	{
		_mesh->vertices[i * VERTEX_ATTRIBUTES]     = importedMesh->mVertices[i].x;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + 1] = importedMesh->mVertices[i].y;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + 2] = importedMesh->mVertices[i].z;

		if (importedMesh->HasTextureCoords(0))
		{
			_mesh->vertices[i * VERTEX_ATTRIBUTES + TEXCOORD_OFFSET]     = importedMesh->mTextureCoords[0][i].x;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + TEXCOORD_OFFSET + 1] = importedMesh->mTextureCoords[0][i].y;

			if (importedMesh->mTangents != nullptr)
			{
				_mesh->vertices[i * VERTEX_ATTRIBUTES + TANGENTS_OFFSET]     = importedMesh->mTangents[i].x;
				_mesh->vertices[i * VERTEX_ATTRIBUTES + TANGENTS_OFFSET + 1] = importedMesh->mTangents[i].y;
				_mesh->vertices[i * VERTEX_ATTRIBUTES + TANGENTS_OFFSET + 2] = importedMesh->mTangents[i].z;
			}
		}
		else
		{
			_mesh->vertices[i * VERTEX_ATTRIBUTES + TEXCOORD_OFFSET]     = 0.0f;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + TEXCOORD_OFFSET + 1] = 0.0f;

			_mesh->vertices[i * VERTEX_ATTRIBUTES + TANGENTS_OFFSET]     = 0.0f;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + TANGENTS_OFFSET + 1] = 0.0f;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + TANGENTS_OFFSET + 2] = 0.0f;
		}

		//Normals
		if (importedMesh->HasNormals())
		{
			_mesh->vertices[i * VERTEX_ATTRIBUTES + NORMALS_OFFSET]     = importedMesh->mNormals[i].x;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + NORMALS_OFFSET + 1] = importedMesh->mNormals[i].y;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + NORMALS_OFFSET + 2] = importedMesh->mNormals[i].z;
			//LOG(LogType::L_NORMAL, "New mesh with %d normals", _mesh->normals_count);
		}
		else
		{
			_mesh->vertices[i * VERTEX_ATTRIBUTES + NORMALS_OFFSET]		= 0.0f;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + NORMALS_OFFSET + 1] = 0.0f;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + NORMALS_OFFSET + 2] = 0.0f;
		}


		//boneIDs
		_mesh->vertices[i * VERTEX_ATTRIBUTES + BONES_ID_OFFSET]     = -1.0f;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + BONES_ID_OFFSET + 1] = -1.0f;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + BONES_ID_OFFSET + 2] = -1.0f;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + BONES_ID_OFFSET + 3] = -1.0f;

		//weights
		_mesh->vertices[i * VERTEX_ATTRIBUTES + WEIGHTS_OFFSET]     = 0.0f;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + WEIGHTS_OFFSET + 1] = 0.0f;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + WEIGHTS_OFFSET + 2] = 0.0f;
		_mesh->vertices[i * VERTEX_ATTRIBUTES + WEIGHTS_OFFSET + 3] = 0.0f;

		//Colors
		if (importedMesh->HasVertexColors(0))
		{
			_mesh->vertices[i * VERTEX_ATTRIBUTES + COLORS_OFFSET]	   = importedMesh->mColors[0]->r;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + COLORS_OFFSET + 1] = importedMesh->mColors[0]->g;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + COLORS_OFFSET + 2] = importedMesh->mColors[0]->b;
		}
		else
		{
			_mesh->vertices[i * VERTEX_ATTRIBUTES + COLORS_OFFSET]     = 0.0f;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + COLORS_OFFSET + 1] = 0.0f;
			_mesh->vertices[i * VERTEX_ATTRIBUTES + COLORS_OFFSET + 2] = 0.0f;
		}
	}

	
	if (importedMesh->HasBones())
	{
		//iterate all bones
		for (size_t boneId = 0; boneId < importedMesh->mNumBones; boneId++)
		{
			aiBone* aibone = importedMesh->mBones[boneId];
			_mesh->bonesMap[aibone->mName.C_Str()] = boneId;

			//Load offsets
			float4x4 offset = float4x4(aibone->mOffsetMatrix.a1, aibone->mOffsetMatrix.a2, aibone->mOffsetMatrix.a3, aibone->mOffsetMatrix.a4,
				                       aibone->mOffsetMatrix.b1, aibone->mOffsetMatrix.b2, aibone->mOffsetMatrix.b3, aibone->mOffsetMatrix.b4,
				                       aibone->mOffsetMatrix.c1, aibone->mOffsetMatrix.c2, aibone->mOffsetMatrix.c3, aibone->mOffsetMatrix.c4,
				                       aibone->mOffsetMatrix.d1, aibone->mOffsetMatrix.d2, aibone->mOffsetMatrix.d3, aibone->mOffsetMatrix.d4);

			//_mesh->bonesOffsets.push_back(offset);
			//Joint* joint = new Joint(aibone->mName.C_Str(), b, offset);
			_mesh->bonesOffsets.push_back(offset);

			//iterate all bone weights
			for (int weights = 0; weights < aibone->mNumWeights; weights++) {

				int vertexId = aibone->mWeights[weights].mVertexId; // *4;

				for (int w = 0; w < 4; w++)
				{
					if (_mesh->vertices[vertexId * VERTEX_ATTRIBUTES + BONES_ID_OFFSET + w] == -1.0f)
					{
						//bone ids
						_mesh->vertices[vertexId * VERTEX_ATTRIBUTES + BONES_ID_OFFSET + w] = boneId;
						//weights
						_mesh->vertices[vertexId * VERTEX_ATTRIBUTES + WEIGHTS_OFFSET  + w] = aibone->mWeights[weights].mWeight;
						break;
					}
				}
			}
			//_mesh->joints.push_back(joint);
		}		
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