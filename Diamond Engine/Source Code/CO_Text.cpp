#include "CO_Text.h"
#include "CO_Transform2D.h"

#include "Application.h"

#include "RE_Material.h"
#include "RE_Shader.h"

#include "MO_FileSystem.h"

#include "IM_FontImporter.h"
#include <map>

#include "MathGeoLib/include/MathGeoLib.h"
#include "ImGui/imgui.h"
#include "OpenGL.h"

C_Text::C_Text(GameObject* gameObject) : Component(gameObject), 
	font_path(""), 
	text(""),
	font(nullptr)
{
	name = "Text";
	font = EngineExternal->moduleFileSystem->free_type_library->GetFont("Arial");
}


C_Text::~C_Text()
{
	font = nullptr;
}


void C_Text::RenderText(C_Transform2D* transform, ResourceMaterial* material, unsigned int VAO, unsigned int VBO)
{
	material->shader->Bind();
	material->PushUniforms();
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	float x = 0;
	float y = 0;

	for (int i = 0; i < text.size(); ++i)
	{
		const Character& character = font->characters.find(text[i])->second;

		glBindTexture(GL_TEXTURE_2D, character.textureId);
		GLint modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "model_matrix");
		glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform->GetGlobal2DTransform().ptr());

		float posX = (x + character.bearing[0]) / 100;
		float posY = -(y + character.bearing[1]) / 100;

		float width = character.size[0] / 100;
		float height = character.size[1] / 100;

		float vertices[6][4] = {
			{ posX,			posY + height,  0.0f, 0.0f },
			{ posX,			posY,			0.0f, 1.0f },
			{ posX + width, posY,			1.0f, 1.0f },

			{ posX,			posY + height,  0.0f, 0.0f },
			{ posX + width, posY,			1.0f, 1.0f },
			{ posX + width, posY + height,  1.0f, 0.0f }
		};

		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (character.advance >> 6);	//Bitshift by 6 to get size in pixels
	}
	
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);

	if (material->shader)
		material->shader->Unbind();
}


#ifndef STANDALONE
bool C_Text::OnEditor()
{
	//EngineExternal->moduleFileSystem->free_type_library
	if (Component::OnEditor() == true)
	{
		char inputText[MAX_CHARACTER_TEXT];
		strcpy(inputText, text.c_str());
		ImGui::InputText("Text to print", &inputText[0], sizeof(inputText));

		text = inputText;
	}
	return true;
}
#endif // !STANDALONE