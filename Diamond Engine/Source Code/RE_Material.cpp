#include "RE_Material.h"
#include "OpenGL.h"
#include"ImGui/imgui.h"

#include"Application.h"
#include"MO_ResourceManager.h"
#include"MO_Scene.h"
#include"RE_Shader.h"
#include"DEJsonSupport.h"
#include"IM_FileSystem.h"

ResourceMaterial::ResourceMaterial(unsigned int _uid) : Resource(_uid, Resource::Type::MATERIAL), shader(nullptr)
{
}

ResourceMaterial::~ResourceMaterial()
{
	uniforms.clear();
	attributes.clear();
}

bool ResourceMaterial::LoadToMemory()
{
	//Load file to buffer [DONE]
	JSON_Value* file = json_parse_file(this->libraryFile.c_str());
	DEConfig base(json_value_get_object(file));

	int shUID = base.ReadInt("ShaderUID");
	json_value_free(file);

	//Request shader [DONE]
	std::string shaderPath = SHADERS_PATH + std::to_string(shUID);
	shaderPath += ".shdr";
	shader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(shUID, shaderPath.c_str()));
	shader->references.push_back(this);

	//Get uniforms and attributes from shader [DONE]
	FillVariables();

	//Load required resources from uniforms


	return false;
}

bool ResourceMaterial::UnloadFromMemory()
{
	//Unload shader by reference count [DONE]
	if(shader != NULL)
		EngineExternal->moduleResources->UnloadResource(shader->GetUID());

	//TODO: Unload resources (uniform and attributes) by reference count 

	uniforms.clear();
	attributes.clear();

	return true;
}

void ResourceMaterial::FillVariables()
{
	uniforms.clear();
	attributes.clear();

	GLint attCount = 0, uniCount = 0;
	glGetProgramiv(shader->shaderProgramID, GL_ACTIVE_ATTRIBUTES, &attCount);

	for (GLint a = 0; a < attCount; a++)
	{
		ShaderVariable shdrVar;
		glGetActiveAttrib(shader->shaderProgramID, (GLuint)a, 25, &shdrVar.nameLength, &shdrVar.vSize, &shdrVar.vType, shdrVar.name);

		attributes.push_back(shdrVar);
	}

	glGetProgramiv(shader->shaderProgramID, GL_ACTIVE_UNIFORMS, &uniCount);
	for (GLint b = 0; b < uniCount; b++)
	{
		ShaderVariable shdrVar;
		glGetActiveUniform(shader->shaderProgramID, (GLuint)b, 25, &shdrVar.nameLength, &shdrVar.vSize, &shdrVar.vType, shdrVar.name);

		uniforms.push_back(shdrVar);
	}
}

void ResourceMaterial::PushUniforms()
{
	//Push all uniforms
	for (size_t i = 0; i < uniforms.size(); ++i)
	{
		//ImGui::SameLine();
		switch (uniforms[i].vType)
		{

		case GL_SAMPLER_2D:

			break;

		case GL_FLOAT_VEC3: 
			glUniform3fv(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), 1, &uniforms[i].data.vector3Value.x);
			break;

		case GL_FLOAT_VEC4:
			break;


		case GL_FLOAT_MAT4:
			break;

		default:
			break;
		}
	}
}

#ifndef STANDALONE
void ResourceMaterial::DrawEditor()
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
			//glGetUniformfv(shaderProgramID, uniforms[i].vIndex, &ret.x);
			ImGui::DragFloat3(uniforms[i].name, &uniforms[i].data.vector3Value.x, 0.005f);
			break;
		}

		case GL_FLOAT_VEC4:
		{
			ImGui::SameLine();
			ImVec4 ret = ImVec4(0, 0, 0, 0);
			ImGui::ColorEdit4("##test", &ret.x);
			break;
		}

		case GL_FLOAT_MAT4:
			ImGui::SameLine();
			ImGui::Text("Matrix here");
			break;

		default:
			break;
		}
	}
}
#endif // !STANDALONE


ShaderVariable::ShaderVariable() : vIndex(0), vType(GL_INT), nameLength(0),
name(""), vSize(0)
{
	//data.vector3Value = float3(0, 0, 0);
	//data.intValue = 0;
}

ShaderVariable::~ShaderVariable()
{
}

ShaderVariable::ShdrValue::ShdrValue() : floatValue(0.0f), intValue(0),
textureValue(0), matrixValue(nullptr), vector3Value(0, 0, 0)
{
}