#include "MO_GUI.h"

#include "Application.h"
#include "MO_Scene.h"
#include "MO_Window.h"
#include "MO_Renderer3D.h"

#include "GameObject.h"
#include "CO_Canvas.h"
#include "CO_Transform2D.h"
#include "CO_Transform.h"
#include "CO_Material.h"
#include "CO_Camera.h"

#include "RE_Material.h"
#include "RE_Shader.h"

#include <stack>

#include "OpenGL.h"

M_Gui::M_Gui(Application* app, bool startEnabled) : Module(app, startEnabled),
	canvas(nullptr), 
	VAO(0),
	index_font(-1)
{
}


M_Gui::~M_Gui()
{
	canvas = nullptr;
}


bool M_Gui::Start()
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8, uiVAO, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	return true;
}


void M_Gui::RenderCanvas2D()
{
	if (canvas != nullptr)
	{
		std::stack<GameObject*> stack;
		GameObject* node = nullptr;

		int elementsCount = canvas->children.size();
		for (int i = 0; i < elementsCount; ++i)
		{
			stack.push(canvas->children[i]);

			while (stack.empty() == false)
			{
				node = stack.top();
				stack.pop();

				RenderUiElement(node);

				int childNumber = node->children.size();
				for (int i = 0; i < childNumber; ++i)
					stack.push(node->children[i]);
			}
		}
	}
}


void M_Gui::RenderCanvas3D() 
{
	
}


void M_Gui::RenderUiElement(GameObject* uiElement)
{
	Component* mat = uiElement->GetComponent(Component::TYPE::MATERIAL);
	Component* trans2D = uiElement->GetComponent(Component::TYPE::TRANSFORM);

	if (mat != nullptr && trans2D != nullptr)
	{
		C_Transform* transform = static_cast<C_Transform*>(trans2D);
		ResourceMaterial* material = static_cast<C_Material*>(mat)->material;

		if (material->shader)
		{
			glEnableClientState(GL_VERTEX_ARRAY);
			material->shader->Bind();
			material->PushUniforms();

			GLint modelLoc = glGetUniformLocation(material->shader->shaderProgramID, "model_matrix");
			glUniformMatrix4fv(modelLoc, 1, GL_FALSE, transform->GetGlobalTransposed());
			glBindVertexArray(VAO);
			glEnableVertexAttribArray(0);
			glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
			glBindVertexArray(0);

			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, 0);

			if (material->shader)
				material->shader->Unbind();

			glDisableClientState(GL_VERTEX_ARRAY);
		}
	}
}


void M_Gui::CreateCanvas()
{
	if (canvas == nullptr)
	{
		canvas = new GameObject("Canvas", App->moduleScene->root);
		canvas->AddComponent(Component::TYPE::CANVAS);
	}
}


void M_Gui::CreateImage()
{
	if (canvas == nullptr)	//TODO Create a GO with a component canvas
		CreateCanvas();

	GameObject* image = new GameObject("Image", canvas);
	image->AddComponent(Component::TYPE::TRANSFORM_2D);
	image->AddComponent(Component::TYPE::MATERIAL);
}


void M_Gui::EraseCanvas()
{
	canvas = nullptr;
}