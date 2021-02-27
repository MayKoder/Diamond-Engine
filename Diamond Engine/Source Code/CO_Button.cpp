#include "CO_Button.h"
#include "CO_Material.h"
#include "RE_Material.h"

#include "Application.h"
#include "GameObject.h"

#include "CO_Script.h"

#include "MO_ResourceManager.h"

#include "ImGui/imgui.h"


#include <assert.h>

C_Button::C_Button(GameObject* gameObject) :Component(gameObject), sprite1(nullptr), sprite2(nullptr), sprite3(nullptr), script(nullptr)
{
	name = "Button";

}

C_Button::~C_Button()
{
	if (sprite1 != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite1->GetUID());
	}
	if (sprite2 != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite2->GetUID());
	}
	if (sprite3 != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite3->GetUID());
	}
}

void C_Button::ChangeSprite(int num_sprite, ResourceMaterial* sprite)
{
	assert(num_sprite >= 1 && num_sprite<=3, "The number of the sprite is not available");
	switch (num_sprite)
	{
	case 1:
		if (sprite1 != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite1->GetUID());
		}
		sprite1 = sprite;
		break;
	case 2:
		if (sprite2 != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite2->GetUID());
		}
		sprite2 = sprite;
		break;
	case 3:
		if (sprite3 != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite3->GetUID());
		}
		sprite3 = sprite;
		break;
	}
}

void C_Button::ChangeScript(C_Script* script)
{
	if (script != nullptr);
}

void C_Button::ExecuteButton()
{
	C_Material* mat = static_cast<C_Material*>(gameObject->GetComponent(TYPE::MATERIAL));
	mat->material = sprite3;
	/// ARNAU: EXECUTE SCRIPT
}

void C_Button::ReleaseButton()
{
	C_Material* mat = static_cast<C_Material*>(gameObject->GetComponent(TYPE::MATERIAL));
	mat->material = sprite1;
}



bool C_Button::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		if (sprite1 != nullptr) {
			ImGui::Text("%s", sprite1->GetAssetPath());
		}
		ImGui::Text("Drop here to change sprite 1");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL"))
			{
				std::string* assetsPath = (std::string*)payload->Data;

				if (sprite1 != nullptr)
					EngineExternal->moduleResources->UnloadResource(sprite1->GetUID());

				sprite1 = dynamic_cast<ResourceMaterial*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath->c_str()));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Separator();

		if (sprite2 != nullptr) {
			ImGui::Text("%s", sprite2->GetAssetPath());
		}
		ImGui::Text("Drop here to change sprite 2");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL"))
			{
				std::string* assetsPath = (std::string*)payload->Data;

				if (sprite2 != nullptr)
					EngineExternal->moduleResources->UnloadResource(sprite2->GetUID());

				sprite2 = dynamic_cast<ResourceMaterial*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath->c_str()));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::Separator();

		if (sprite3 != nullptr) {
			ImGui::Text("%s", sprite3->GetAssetPath());
		}
		ImGui::Text("Drop here to change sprite 3");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_MATERIAL"))
			{
				std::string* assetsPath = (std::string*)payload->Data;

				if (sprite3 != nullptr)
					EngineExternal->moduleResources->UnloadResource(sprite3->GetUID());

				sprite3 = dynamic_cast<ResourceMaterial*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath->c_str()));
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::Separator();

		ImGui::Text("Drop here to change the script");
		/// ARNAU: Finish the script things
		/*if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCRIPT"))
			{
				std::string* assetsPath = (std::string*)payload->Data;

				if (script != nullptr)
					delete script;

				script = dynamic_cast<C_Script*>(gameObject->AddComponent(TYPE::SCRIPT, "");

			}
			ImGui::EndDragDropTarget();
		}*/


	}
	return true;
}
