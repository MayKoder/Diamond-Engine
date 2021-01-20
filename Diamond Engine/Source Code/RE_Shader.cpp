#include "RE_Shader.h"
#include"OpenGL.h"
#include"Globals.h"
#include"IM_FileSystem.h"
#include"IM_ShaderImporter.h"

#include"ImGui/imgui.h"

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

void ResourceShader::Bind()
{
	glUseProgram(shaderProgramID);
}

void ResourceShader::Unbind()
{
	glUseProgram(0);
}

#ifndef STANDALONE
void ResourceShader::DrawEditor()
{
	ImGui::Dummy(ImVec2(0, 5));
	ImGui::Text("Attributes");
	for (size_t i = 0; i < attributes.size(); i++)
	{
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s: ", attributes[i].name);
	}

	ImGui::Dummy(ImVec2(0, 5));
	ImGui::Text("Uniforms");
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s: ", uniforms[i].name);

		//ImGui::SameLine();
		switch (uniforms[i].vType)
		{

		case GL_SAMPLER_2D: 
		{
			//ImGui::SameLine();
			//GLuint test = 0;
			//glGetUniformuiv(shaderProgramID, uniforms[i].vIndex, &test);
			//ImGui::Image((ImTextureID)test, ImVec2(50, 50));
			break;
		}

		case GL_FLOAT_VEC3: 
		{
			ImGui::SameLine();
			ImVec4 ret = ImVec4(0, 0, 0, 0);
			glGetUniformfv(shaderProgramID, uniforms[i].vIndex, &ret.x);
			ImGui::ColorEdit4("##test", &ret.x);
			break;
		}
		default:
			break;
		}
	}
}
#endif // !STANDALONE

void ResourceShader::FillVariables()
{
	GLint attCount = 0, uniCount = 0;
	glGetProgramiv(shaderProgramID, GL_ACTIVE_ATTRIBUTES, &attCount);

	for (size_t a = 0; a < attCount; a++)
	{
		ShaderVariable shdrVar;
		glGetActiveAttrib(shaderProgramID, (GLuint)a, 25, &shdrVar.nameLength, &shdrVar.vSize, &shdrVar.vType, shdrVar.name);

		attributes.push_back(shdrVar);
	}

	glGetProgramiv(shaderProgramID, GL_ACTIVE_UNIFORMS, &uniCount);
	for (size_t b = 0; b < uniCount; b++)
	{
		ShaderVariable shdrVar;
		glGetActiveUniform(shaderProgramID, (GLuint)b, 25, &shdrVar.nameLength, &shdrVar.vSize, &shdrVar.vType, shdrVar.name);

		uniforms.push_back(shdrVar);
	}
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
	std::string hola(cursor);
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
	FillVariables();


	RELEASE_ARRAY(vertex);
	RELEASE_ARRAY(fragment);
	RELEASE_ARRAY(fileBuffer);
}

ShaderVariable::ShaderVariable() : vIndex(0), data(nullptr), vType(0), nameLength(0),
name(""), vSize(0)
{
}
