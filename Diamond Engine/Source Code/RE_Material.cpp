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
#include "RE_Texture.h"

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

	if (shader != nullptr)
	{
		shader->references.push_back(this);

		//Get uniforms and attributes from shader [DONE]
		FillVariables();
	}

	//Load required resources from uniforms
	JSON_Array* uniformsArray = base.ReadArray("uniforms");

	json_value_free(file);

	return false;
}

bool ResourceMaterial::UnloadFromMemory()
{
	//Unload shader by reference count [DONE]
	if (shader != NULL)
		EngineExternal->moduleResources->UnloadResource(shader->GetUID());

	//TODO: Unload resources (uniform and attributes) by reference count 
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		if(uniforms[i].vType == GL_SAMPLER_2D && uniforms[i].data.textureValue != nullptr)
			EngineExternal->moduleResources->UnloadResource(uniforms[i].data.textureValue->GetUID());
	}

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
	int used_textures = 0;
	//Push all uniforms
	for (size_t i = 0; i < uniforms.size(); ++i)
	{
		//ImGui::SameLine();
		switch (uniforms[i].vType)
		{
		case GL_SAMPLER_2D:

			glActiveTexture(GL_TEXTURE0 + used_textures);
			glUniform1i(glGetUniformLocation(shader->shaderProgramID, uniforms[i].name), used_textures);

			if (uniforms[i].data.textureValue != nullptr) {
				glBindTexture(GL_TEXTURE_2D, uniforms[i].data.textureValue->textureID);}
			else {
				glBindTexture(GL_TEXTURE_2D, EngineExternal->moduleRenderer3D->checkersTexture);}

			used_textures++;
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

bool ResourceMaterial::IsDefaultUniform(const char* uniform_name)
{
	for (size_t df = 0; df < sizeof(defaultUniforms) / sizeof(defaultUniforms[0]); df++)
	{
		if (strcmp(uniform_name, defaultUniforms[df]) == 0)
		{
			return true;
		}
	}

	return false;
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
		if (IsDefaultUniform(uniforms[i].name))
			continue;

		ImGui::TextColored(ImVec4(1.f, 1.f, 0.f, 1.f), "%s: ", uniforms[i].name);

		//ImGui::SameLine();
		switch (uniforms[i].vType)
		{

		case GL_SAMPLER_2D:
		{
			//DragDrop texture
			if (uniforms[i].data.textureValue != nullptr)
			{
				ImGui::Image((ImTextureID)uniforms[i].data.textureValue->textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
			}
			else
			{
				ImGui::Image((ImTextureID)0, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));
			}

			if (ImGui::BeginDragDropTarget())
			{
				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
				{
					//Drop asset from Asset window to scene window
					std::string* metaFileDrop = (std::string*)payload->Data;

					if (uniforms[i].data.textureValue != nullptr)
						EngineExternal->moduleResources->UnloadResource(uniforms[i].data.textureValue->GetUID());

					std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

					uniforms[i].data.textureValue = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
					LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
				}
				ImGui::EndDragDropTarget();
			}

			if (uniforms[i].data.textureValue != nullptr)
			{
				char name[32];
				sprintf_s(name, "Remove Texture: %d", uniforms[i].data.textureValue->GetUID());
				if (ImGui::Button(name))
				{
					EngineExternal->moduleResources->UnloadResource(uniforms[i].data.textureValue->GetUID());
					uniforms[i].data.textureValue = nullptr;
				}
			}

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
			ImGui::DragFloat4(uniforms[i].name, &uniforms[i].data.vector4Value.x, 0.005f);
			//ImVec4 ret = ImVec4(0, 0, 0, 0);
			//ImGui::ColorEdit4("##test", &ret.x);
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
void ResourceMaterial::SaveToJson(JSON_Array* uniformsArray)
{
	for (size_t i = 0; i < uniforms.size(); i++)
	{
		if (IsDefaultUniform(uniforms[i].name))
			continue;

		JSON_Value* uniformValue = json_value_init_object();
		DEConfig uniformObject(json_value_get_object(uniformValue));

		uniformObject.WriteString("name", uniforms[i].name);
		uniformObject.WriteInt("type", uniforms[i].vType);

		switch (uniforms[i].vType)
		{
			case GL_SAMPLER_2D:
				if (uniforms[i].data.textureValue != nullptr)
				{
					uniformObject.WriteInt("value", uniforms[i].data.textureValue->GetUID());
				}
				else
				{
					uniformObject.WriteInt("value", 0);
				}
				break;
		

			case GL_INT:
				uniformObject.WriteInt("value", uniforms[i].data.intValue);
				break;

			case GL_FLOAT:
				uniformObject.WriteFloat("value", uniforms[i].data.floatValue);
				break;

			case GL_FLOAT_VEC2:
				uniformObject.WriteVector2("value", &uniforms[i].data.vector2Value.x);
				break;

			case GL_FLOAT_VEC3:
				uniformObject.WriteVector3("value", &uniforms[i].data.vector3Value.x);
				break;
	
			case GL_FLOAT_VEC4:
				uniformObject.WriteVector4("value", &uniforms[i].data.vector4Value.x);
				break;
		
			default:
				break;
		}

		json_array_append_value(uniformsArray, uniformValue);
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
{}

ShaderVariable::ShdrValue::ShdrValue() : intValue(0), floatValue(0.0f), vector2Value(0,0), vector3Value(0, 0, 0), vector4Value(0,0,0,0),
textureValue(nullptr), matrixValue(nullptr)
{}