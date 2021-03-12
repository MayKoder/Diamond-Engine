#include "CO_Image2D.h"
#include "RE_Texture.h"
#include "RE_Material.h"
#include "RE_Shader.h"

#include "Application.h"
#include "MO_ResourceManager.h"
#include "MO_Renderer3D.h"

#include "ImGui/imgui.h"
#include "OpenGL.h"

C_Image2D::C_Image2D(GameObject* gameObject) : Component(gameObject),
	texture(nullptr)
{
	name = "Image 2D";
}


C_Image2D::~C_Image2D()
{
	if (texture != nullptr)
		EngineExternal->moduleResources->UnloadResource(texture->GetUID());
}


#ifndef STANDALONE
bool C_Image2D::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		if (texture != nullptr)
			ImGui::Image((ImTextureID)texture->textureID, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));

		else
			ImGui::Image((ImTextureID)0, ImVec2(64, 64), ImVec2(0, 1), ImVec2(1, 0));


		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				//Drop asset from Asset window to scene window
				std::string* metaFileDrop = (std::string*)payload->Data;

				if (texture != nullptr)
					EngineExternal->moduleResources->UnloadResource(texture->GetUID());

				std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

				texture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
				LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
			}
			ImGui::EndDragDropTarget();
		}

		if (texture != nullptr)
		{
			char name[32];
			sprintf_s(name, "Remove Texture: %d", texture->GetUID());
			if (ImGui::Button(name))
			{
				EngineExternal->moduleResources->UnloadResource(texture->GetUID());
				texture = nullptr;
			}
		}
	}

	return true;
}
#endif // !STANDALONE


void C_Image2D::RenderImage(float* transform, ResourceMaterial* material, unsigned int VAO)
{
	material->shader->Bind();
	material->PushUniforms();

	if (texture != nullptr)
		glBindTexture(GL_TEXTURE_2D, texture->textureID);

	GLint modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "model_matrix");
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform);

	glBindBuffer(GL_ARRAY_BUFFER, VAO);
	//glVertexPointer(2, GL_FLOAT, 0, NULL);
	glEnableVertexAttribArray(0);            
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);

	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	//glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (material->shader)
		material->shader->Unbind();
}


ResourceTexture* C_Image2D::GetTexture() const
{
	return texture;
}


void C_Image2D::SetTexture(ResourceTexture* tex)
{
	EngineExternal->moduleResources->RequestResource(tex->GetUID(), tex->GetLibraryPath());

	if (texture != nullptr)
		EngineExternal->moduleResources->UnloadResource(texture->GetUID());

	texture = tex;
}


void C_Image2D::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	if (texture != nullptr)
	{
		DEJson::WriteString(nObj, "AssetPath", texture->GetAssetPath());
		DEJson::WriteString(nObj, "LibraryPath", texture->GetLibraryPath());
		DEJson::WriteInt(nObj, "UID", texture->GetUID());
	}
}


void C_Image2D::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	std::string texPath = nObj.ReadString("AssetPath");
	std::string texName = nObj.ReadString("LibraryPath");

	if (texName != "")
		texture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("UID"), texName.c_str()));
}