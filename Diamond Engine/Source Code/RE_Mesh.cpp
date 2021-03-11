#include "RE_Mesh.h"
#include "OpenGL.h"
#include "MeshArrays.h"
#include "IM_FileSystem.h"
#include"Globals.h"
#include"RE_Shader.h"
#include"RE_Material.h"

#include"Application.h"
#include"MO_Scene.h" //This can be removed
#include"MO_Camera3D.h" //This can be deleted
#include"CO_Transform.h"
#include"DETime.h"
#include "GameObject.h"
#include"CO_DirectionalLight.h"

ResourceMesh::ResourceMesh(unsigned int _uid) : Resource(_uid, Resource::Type::MESH), indices_id(0), vertices_id(0), generalWireframe(nullptr),
EBO(0), VAO(0), VBO(0)
{

}

ResourceMesh::~ResourceMesh()
{}

bool ResourceMesh::LoadToMemory()
{
	LOG(LogType::L_WARNING, "Mesh loaded to memory"); //UNCOMMENT
	LoadCustomFormat(GetLibraryPath());

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, (GLuint*)&(VBO));
	glGenBuffers(1, (GLuint*)&(EBO));

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * VERTEX_ATTRIBUTES * vertices_count, vertices, GL_STATIC_DRAW);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint) * indices_count, indices, GL_STATIC_DRAW);

	//position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	//texcoords attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	//normals attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(5 * sizeof(GLfloat)));
	glEnableVertexAttribArray(2);

	//tangents attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, VERTEX_ATTRIBUTES * sizeof(float), (GLvoid*)(8 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	return true;
}

bool ResourceMesh::UnloadFromMemory()
{
	glDeleteVertexArrays(1, &VAO);
	VAO = 0u;
	glDeleteBuffers(1, &VBO);
	VBO = 0u;
	glDeleteBuffers(1, &EBO);
	EBO = 0u;

	//Clear buffers
	if (indices != nullptr)
		delete[] indices;

	if (vertices != nullptr)
		delete[] vertices;

	//Restart to nullptr
	indices = nullptr;
	vertices = nullptr;

	return true;
}

void ResourceMesh::RenderMesh(GLuint textureID, float3 color, bool renderTexture, ResourceMaterial* material, C_Transform* _transform)
{
	//ASK: glDrawElementsInstanced()?
	//if (textureID != 0 && (renderTexture || (generalWireframe != nullptr && *generalWireframe == false)))
		//glBindTexture(GL_TEXTURE_2D, textureID);
	
	if (material->shader)
	{
		material->shader->Bind();
		material->PushUniforms();

		if(textureID != 0)
			glUniform1i(glGetUniformLocation(material->shader->shaderProgramID, "hasTexture"), 1);
		else
			glUniform1i(glGetUniformLocation(material->shader->shaderProgramID, "hasTexture"), 0);

		GLint modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "model_matrix");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, _transform->GetGlobalTransposed());

		modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "view");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, EngineExternal->moduleRenderer3D->activeRenderCamera->ViewMatrixOpenGL().ptr());

		modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "projection");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, EngineExternal->moduleRenderer3D->activeRenderCamera->ProjectionMatrixOpenGL().ptr());

		modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "normalMatrix");
		float3x3 normalMatrix = _transform->globalTransform.Float3x3Part().Transposed();
		normalMatrix.Inverse();
		normalMatrix.Transpose();
		glUniformMatrix3fv(modelLoc, 1, GL_FALSE, normalMatrix.ptr());

		modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "time");
		glUniform1f(modelLoc, DETime::realTimeSinceStartup);

		modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "cameraPosition");
		float3 cameraPosition = EngineExternal->moduleRenderer3D->activeRenderCamera->GetPosition();
		glUniform3fv(modelLoc, 1, &cameraPosition.x);

		modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "altColor");
		glUniform3fv(modelLoc, 1, &color.x);

		if(EngineExternal->moduleRenderer3D->directLight)
		EngineExternal->moduleRenderer3D->directLight->PushLightUniforms(material);
	}

	OGL_GPU_Render();

	//if (textureID != 0 && (renderTexture || (generalWireframe != nullptr && *generalWireframe == false)))
	//{
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	//}

	if (material->shader)
		material->shader->Unbind();
}

void ResourceMesh::OGL_GPU_Render()
{
	//vertices
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices_count, GL_UNSIGNED_INT, NULL);
	glBindVertexArray(0);
}

void ResourceMesh::RenderMeshDebug(bool* vertexNormals, bool* faceNormals, const float* globalTransform)
{
	glPushMatrix();
	glMultMatrixf(globalTransform);
	
	if (*vertexNormals == true)
	{
		float normalLenght = 0.25f;
		glPointSize(3.0f);
		glColor3f(1, 0, 0);
		glBegin(GL_POINTS);
		for (unsigned int i = 0; i < vertices_count; i ++)
		{
			glVertex3f(vertices[i * VERTEX_ATTRIBUTES], vertices[i * VERTEX_ATTRIBUTES + 1], vertices[i * VERTEX_ATTRIBUTES + 2]);
		}
		glEnd();
		glColor3f(0, 1, 0);
		glPointSize(1.0f);

		//Vertex normals
		glColor3f(0, 1, 0);
		glBegin(GL_LINES);
		for (unsigned int i = 0; i < vertices_count; i++)
		{
			glVertex3f(vertices[i * VERTEX_ATTRIBUTES], vertices[i * VERTEX_ATTRIBUTES + 1], vertices[i * VERTEX_ATTRIBUTES + 2]);
			glVertex3f(vertices[i * VERTEX_ATTRIBUTES] + vertices[i * VERTEX_ATTRIBUTES + 5] * normalLenght,
					   vertices[i * VERTEX_ATTRIBUTES + 1] + vertices[i * VERTEX_ATTRIBUTES + 6] * normalLenght,
					   vertices[i * VERTEX_ATTRIBUTES + 2] + vertices[i * VERTEX_ATTRIBUTES + 7] * normalLenght);
		}
		glEnd();
		glColor3f(1, 1, 1);
	}

	if (*faceNormals == true)
	{
		float normalLenght = 0.25f;
		//Face normals
		glColor3f(1, 0, 0);
		glBegin(GL_LINES);
		for (int i = 0; i < indices_count; i += 3)
		{
			/*vec3 A = GetVectorFromIndex(&vertices[indices[i] * 3]);
			vec3 B = GetVectorFromIndex(&vertices[indices[i + 1] * 3]);
			vec3 C = GetVectorFromIndex(&vertices[indices[i + 2] * 3]);*/

			vec3 A = GetVectorFromIndex(&vertices[indices[i] * VERTEX_ATTRIBUTES]);
			vec3 B = GetVectorFromIndex(&vertices[indices[i + 1] * VERTEX_ATTRIBUTES]);
			vec3 C = GetVectorFromIndex(&vertices[indices[i + 2] * VERTEX_ATTRIBUTES]);

			vec3 middle((A.x + B.x + C.x) / 3.f, (A.y + B.y + C.y) / 3.f, (A.z + B.z + C.z) / 3.f);

			vec3 crossVec = cross((B - A), (C - A));
			vec3 normalDirection = normalize(crossVec);

			glVertex3f(middle.x, middle.y, middle.z);
			glVertex3f(middle.x + normalDirection.x * normalLenght, middle.y + normalDirection.y * normalLenght, middle.z + normalDirection.z * normalLenght);
		}
		glEnd();
		glPointSize(1.f);
		glColor3f(1, 1, 1);
	}

	glPopMatrix();
}

vec3 ResourceMesh::GetVectorFromIndex(float* startValue)
{
	float x = *startValue;
	++startValue;
	float y = *startValue;
	++startValue;
	float z = *startValue;

	return vec3(x, y, z);
}

const char* ResourceMesh::SaveCustomFormat(uint& retSize)
{
	uint aCounts[2] = { indices_count, vertices_count};
	retSize = sizeof(aCounts) + (sizeof(uint) * indices_count) + (sizeof(float) * vertices_count * VERTEX_ATTRIBUTES);

	char* fileBuffer = new char[retSize];
	char* cursor = fileBuffer;

	uint bytes = sizeof(aCounts);
	memcpy(cursor, aCounts, bytes);
	cursor += bytes;

	bytes = sizeof(uint) * indices_count;
	memcpy(cursor, indices, bytes);
	cursor += bytes;

	bytes = sizeof(float) * vertices_count * VERTEX_ATTRIBUTES;
	memcpy(cursor, vertices, bytes);
	cursor += bytes;

	return fileBuffer;
}

void ResourceMesh::LoadCustomFormat(const char* path)
{
	char* fileBuffer = nullptr;

	uint size = FileSystem::LoadToBuffer(path, &fileBuffer);

	if (size == 0)
		return;

	char* cursor = fileBuffer;
	uint variables[2];

	uint bytes = sizeof(variables);
	memcpy(variables, cursor, bytes);
	indices_count = variables[0];
	vertices_count = variables[1];
	cursor += bytes;

	bytes = sizeof(uint) * indices_count;

	indices = new uint[indices_count];
	memcpy(indices, cursor, bytes);
	cursor += bytes;

	vertices = new float[vertices_count * VERTEX_ATTRIBUTES];
	bytes = sizeof(float) * vertices_count * VERTEX_ATTRIBUTES;
	memcpy(vertices, cursor, bytes);
	cursor += bytes;

	//TODO: Should this be here?
	float* vertices_positions = new float[vertices_count * 3];
	for (size_t i = 0; i < vertices_count; i++)
	{
		vertices_positions[i * 3] = vertices[i * VERTEX_ATTRIBUTES];
		vertices_positions[i * 3 + 1] = vertices[i * VERTEX_ATTRIBUTES + 1];
		vertices_positions[i * 3 + 2] = vertices[i * VERTEX_ATTRIBUTES + 2];
	}

	localAABB.SetNegativeInfinity();
	localAABB.Enclose((float3*)vertices_positions, vertices_count);
	delete[] vertices_positions;

	delete[] fileBuffer;
	fileBuffer = nullptr;
}
