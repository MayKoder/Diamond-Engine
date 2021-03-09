#include "RE_Shader.h"
#include"OpenGL.h"
#include"Globals.h"
#include"IM_FileSystem.h"
#include"IM_ShaderImporter.h"

#include"ImGui/imgui.h"
#include"RE_Material.h"

ResourceShader::ResourceShader(unsigned int _uid) : Resource(_uid, Resource::Type::SHADER), shaderProgramID(0)
{
	for (GLuint i = 0; i < static_cast<GLuint>(ShaderType::SH_Max); i++)
		shaderObjects[i] = 0;
}

ResourceShader::~ResourceShader()
{
	references.clear();
	if (shaderProgramID != 0) 
	{
		glDeleteShader(shaderProgramID);
		shaderProgramID = 0;
	}
}

void ResourceShader::LinkToProgram()
{
	shaderProgramID = glCreateProgram();

	for (size_t i = 0; i < static_cast<int>(ShaderType::SH_Max); i++)
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

	for (size_t i = 0; i < static_cast<int>(ShaderType::SH_Max); i++)
	{
		glDetachShader(shaderProgramID, shaderObjects[i]);
	}

	for (size_t i = 0; i < static_cast<int>(ShaderType::SH_Max); i++)
	{
		glDeleteShader(shaderObjects[i]);
		shaderObjects[i] = 0;
	}

	//TODO: Find all the active materials using this shader and update their uniforms with material->FillVariables()
	//TODO: What if the shader changes on some materials?
	for (std::vector<ResourceMaterial*>::iterator i = references.begin(); i != references.end(); ++i)
	{
		(*i)->FillVariables();
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

void ResourceShader::Bind()
{
	glUseProgram(shaderProgramID);
}

void ResourceShader::Unbind()
{
	glUseProgram(0);
}

char* ResourceShader::SaveShaderCustomFormat(char* vertexObjectBuffer, int vofSize, char* fragObjectBuffer, int fobSize)
{
	int aCounts[2] = { vofSize, fobSize};

	int retSize = sizeof(aCounts) + (sizeof(char) * vofSize) + (sizeof(char) * fobSize);

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


	bytes = sizeof(char) * variables[(int)ShaderType::SH_Vertex];
	char* vertex = new char[variables[(int)ShaderType::SH_Vertex]];
	ZeroMemory(vertex, bytes);
	memcpy(vertex, cursor, bytes);
	cursor += bytes;

	bytes = sizeof(char) * variables[(int)ShaderType::SH_Frag];
	char* fragment = new char[bytes];
	ZeroMemory(fragment, bytes);
	memcpy(fragment, cursor, bytes);

	shaderObjects[(int)ShaderType::SH_Vertex] = ShaderImporter::Compile(vertex, ShaderType::SH_Vertex, variables[(int)ShaderType::SH_Vertex]);
	shaderObjects[(int)ShaderType::SH_Frag] = ShaderImporter::Compile(fragment, ShaderType::SH_Frag, variables[(int)ShaderType::SH_Frag]);

	this->LinkToProgram();

	RELEASE_ARRAY(vertex);
	RELEASE_ARRAY(fragment);
	RELEASE_ARRAY(fileBuffer);
}
