#include "RE_Shader.h"
#include"OpenGL.h"
#include"Globals.h"
#include"IM_FileSystem.h"
#include"IM_ShaderImporter.h"

ResourceShader::ResourceShader(unsigned int _uid) : Resource(_uid, Resource::Type::SHADER), shaderProgramID(0)
{
	for (GLuint i = 0; i < static_cast<GLuint>(ShaderType::SH_Max); i++)
		shaderObjects[i] = 0;


}

ResourceShader::~ResourceShader()
{
	if (shaderProgramID != 0) 
	{
		glDeleteShader(shaderProgramID);
		shaderProgramID = 0;
	}
}

void ResourceShader::LinkToProgram()
{
	shaderProgramID = glCreateProgram();

	for (size_t i = 0; i < ShaderType::SH_Max; i++)
	{
		glAttachShader(shaderProgramID,  shaderObjects[i]);
	}

	glLinkProgram(shaderProgramID);

	int success;
	char infoLog[512];
	glGetProgramiv(shaderProgramID, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetProgramInfoLog(shaderProgramID, 512, NULL, infoLog);
		LOG(LogType::L_ERROR, "Error linking shader program: %s", infoLog);
	}
	//else
	//{
	//	shader->program_id = shaderProgram;

	//	SetUniforms(shaderProgram, shader);

	//	LOG("Shader program created porperly");
	//}

	for (size_t i = 0; i < ShaderType::SH_Max; i++)
	{
		glDetachShader(shaderProgramID, shaderObjects[i]);
	}

	for (size_t i = 0; i < ShaderType::SH_Max; i++)
	{
		glDeleteShader(shaderObjects[i]);
		shaderObjects[i] = 0;
	}
}

bool ResourceShader::LoadToMemory()
{
	LoadShaderCustomFormat(libraryFile.c_str());
	return false;
}

bool ResourceShader::UnloadFromMemory()
{
	for (size_t i = 0; i < static_cast<size_t>(ShaderType::SH_Max); i++)
	{
		glDeleteShader(shaderObjects[i]);
		shaderObjects[i] = 0;
	}

	glDeleteShader(shaderProgramID);
	shaderProgramID = 0;

	return true;
}

char* ResourceShader::SaveShaderCustomFormat(char* vertexObjectBuffer, int vofSize, char* fragObjectBuffer, int fobSize)
{

	int aCounts[2] = { vofSize, fobSize};

	int retSize = sizeof(aCounts) + vofSize + fobSize;

	char* fileBuffer = new char[retSize];
	char* cursor = fileBuffer;

	uint bytes = sizeof(aCounts);
	memcpy(cursor, aCounts, bytes); //TODO WARNING: What does this mean lol?
	cursor += bytes;

	bytes = vofSize;
	memcpy(cursor, vertexObjectBuffer, bytes);
	cursor += bytes;

	bytes = fobSize;
	memcpy(cursor, fragObjectBuffer, bytes);
	cursor += bytes;

	return fileBuffer;
}

void ResourceShader::LoadShaderCustomFormat(const char* libraryPath)
{
	char* fileBuffer = nullptr;

	uint size = FileSystem::LoadToBuffer(libraryPath, &fileBuffer);

	if (size == 0)
		return;

	char* cursor = fileBuffer;
	uint variables[2];

	uint bytes = sizeof(variables);
	memcpy(variables, cursor, bytes);
	cursor += bytes;


	bytes = sizeof(char) * variables[ShaderType::SH_Vertex];
	char* vertex = new char[variables[ShaderType::SH_Vertex]];
	memcpy(vertex, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(char) * variables[ShaderType::SH_Frag];
	char* fragment = new char[variables[ShaderType::SH_Frag]];
	memcpy(fragment, cursor, bytes);

	shaderObjects[ShaderType::SH_Vertex] = ShaderImporter::Compile(vertex, ShaderType::SH_Vertex);
	shaderObjects[ShaderType::SH_Frag] = ShaderImporter::Compile(fragment, ShaderType::SH_Frag);

	this->LinkToProgram();

	RELEASE_ARRAY(fileBuffer);
	RELEASE_ARRAY(vertex);
	RELEASE_ARRAY(fragment);
}
