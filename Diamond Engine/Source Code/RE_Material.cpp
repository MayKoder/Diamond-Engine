#include "RE_Material.h"
#include "OpenGL.h"
#include"ImGui/imgui.h"

#include"Application.h"
#include"MO_ResourceManager.h"
#include"MO_Scene.h"
#include"RE_Shader.h"
#include"DEJsonSupport.h"
#include"IM_FileSystem.h"
#include"DEJsonSupport.h"

ResourceMaterial::ResourceMaterial(unsigned int _uid) : Resource(_uid, Resource::Type::MATERIAL), shader(nullptr) {}

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

	//Request shader [DONE]
	std::string shaderPath = SHADERS_PATH + std::to_string(shUID);
	shaderPath += ".shdr";
	shader = dynamic_cast<ResourceShader*>(EngineExternal->moduleResources->RequestResource(shUID, shaderPath.c_str()));
	shader->references.push_back(this);

	//Get uniforms and attributes from shader [DONE]
	FillVariables();

	//Load required resources from uniforms

	json_value_free(file);

	return false;
}

bool ResourceMaterial::UnloadFromMemory()
{
	//Unload shader by reference count [DONE]
	if (shader != NULL)
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

		case GL_INT:
			glUniform1i(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.intValue);
			break;

		case GL_FLOAT:
			glUniform1f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.floatValue);
			break;

		case GL_FLOAT_VEC2:
			glUniform2f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.vector2Value.x, uniforms[i].data.vector2Value.y);
			break;

		case GL_FLOAT_VEC3:
			glUniform3f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.vector3Value.x,
				uniforms[i].data.vector3Value.y, uniforms[i].data.vector3Value.z);
			break;

		case GL_FLOAT_VEC4:
			glUniform4f(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), uniforms[i].data.vector4Value.x,
				uniforms[i].data.vector4Value.y, uniforms[i].data.vector4Value.z, uniforms[i].data.vector4Value.w);
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
		//Check it is not a default uniform, if it is, hide it 
		bool defaultUniform = false;
		for (size_t df = 0; df < sizeof(defaultUniforms) / sizeof(defaultUniforms[0]); df++)
		{
			if (strcmp(uniforms[i].name, defaultUniforms[df]) == 0)
			{
				defaultUniform = true;
				break;
			}
		}

		if (defaultUniform)
			continue;

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

		case GL_INT:
			ImGui::SameLine();
			ImGui::DragInt(uniforms[i].name, &uniforms[i].data.intValue, 1);
			break;

		case GL_FLOAT:
			ImGui::SameLine();
			ImGui::DragFloat(uniforms[i].name, &uniforms[i].data.floatValue, 0.005f);
			break;

		case GL_FLOAT_VEC2:
			ImGui::SameLine();
			ImGui::DragFloat2(uniforms[i].name, &uniforms[i].data.vector2Value.x, 0.005f);
			break;

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
void ResourceMaterial::SaveToJson(JSON_Array* json)
{
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		JSON_Value* uniformValue = json_value_init_object();
		JSON_Object* uniformData = json_value_get_object(uniformValue);

		json_object_set_string(uniformData, "name", uniforms[i].name);
		json_object_set_number(uniformData, "type", uniforms[i].vType);

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

		case GL_INT:
			DEJson::WriteInt(uniformData, "value", uniforms[i].data.intValue);
			break;

		case GL_FLOAT:
			DEJson::WriteFloat(uniformData, "value", uniforms[i].data.floatValue);
			break;

		case GL_FLOAT_VEC2:
			DEJson::WriteVector2(uniformData, "value", &uniforms[i].data.vector2Value.x);
			break;

		case GL_FLOAT_VEC3:
			DEJson::WriteVector3(uniformData, "value", &uniforms[i].data.vector3Value.x);
			break;
	
		case GL_FLOAT_VEC4:
			DEJson::WriteVector4(uniformData, "value", &uniforms[i].data.vector4Value.x);
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