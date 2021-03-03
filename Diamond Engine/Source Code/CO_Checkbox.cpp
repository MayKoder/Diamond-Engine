#include "CO_Checkbox.h"
#include "CO_Script.h"
#include "CO_Image2D.h"

#include "RE_Texture.h"

#include "GameObject.h"

#include "Application.h"
#include "MO_ResourceManager.h"

#include "IM_FileSystem.h"

#include "ImGui/imgui.h"

#include <assert.h>

C_Checkbox::C_Checkbox(GameObject* gameObject): Component(gameObject), sprite_checkbox_active(nullptr), sprite_checkbox_active_hovered(nullptr), sprite_checkbox_active_pressed(nullptr),
sprite_checkbox_unactive(nullptr), sprite_checkbox_unactive_hovered(nullptr), sprite_checkbox_unactive_pressed(nullptr), script(nullptr), num_sprite_used(CHECKBOXSTATE::CHECKBOXUNACTIVE),
checkbox_active(false)
{
	name = "Button";
}

C_Checkbox::~C_Checkbox()
{
	if (sprite_checkbox_active != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite_checkbox_active->GetUID());
	}
	if (sprite_checkbox_active_hovered != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite_checkbox_active_hovered->GetUID());
	}
	if (sprite_checkbox_active_pressed != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite_checkbox_active_pressed->GetUID());
	}
	if (sprite_checkbox_unactive != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite_checkbox_unactive->GetUID());
	}
	if (sprite_checkbox_unactive_hovered != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite_checkbox_unactive_hovered->GetUID());
	}
	if (sprite_checkbox_unactive_pressed != nullptr) {
		EngineExternal->moduleResources->UnloadResource(sprite_checkbox_unactive_pressed->GetUID());
	}
}

void C_Checkbox::Update()
{
#ifndef STANDALONE
	ChangeTexture(num_sprite_used);
#endif // !STANDALONE
}

void C_Checkbox::PressCheckbox()
{
	if (checkbox_active) {
		ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVEPRESSED);
	}
	else {
		ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVEPRESSED);
	}
}

void C_Checkbox::UnpressCheckbox()
{
	checkbox_active = !checkbox_active;
	if (checkbox_active) {
		/// ARNAU: EXECUTE SCRIPT
		ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVEHOVERED);
	}
	else {
		/// ARNAU: EXECUTE SCRIPT
		ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED);
	}
}

void C_Checkbox::ChangeTexture(CHECKBOXSTATE new_num_sprite)
{
	num_sprite_used = new_num_sprite;
	switch (new_num_sprite)
	{
	case CHECKBOXSTATE::CHECKBOXACTIVE:
	{
		if (sprite_checkbox_active == nullptr)
		{
			LOG(LogType::L_WARNING, "The sprite 'A' is nullptr");
			return;
		}
		C_Image2D* img = static_cast<C_Image2D*>(gameObject->GetComponent(TYPE::IMAGE_2D));

		if (img != nullptr)
			img->SetTexture(sprite_checkbox_active);
		break;
	}

	case CHECKBOXSTATE::CHECKBOXACTIVEHOVERED:
	{
		if (sprite_checkbox_active_hovered == nullptr) {
			LOG(LogType::L_WARNING, "The sprite 'AH' is nullptr");
			return;
		}

		C_Image2D* img = static_cast<C_Image2D*>(gameObject->GetComponent(TYPE::IMAGE_2D));

		if (img != nullptr)
			img->SetTexture(sprite_checkbox_active_hovered);
		break;
	}
	case CHECKBOXSTATE::CHECKBOXACTIVEPRESSED:
	{
		if (sprite_checkbox_active_pressed == nullptr) {
			LOG(LogType::L_WARNING, "The sprite 'AP' is nullptr");
			return;
		}

		C_Image2D* img = static_cast<C_Image2D*>(gameObject->GetComponent(TYPE::IMAGE_2D));

		if (img != nullptr)
			img->SetTexture(sprite_checkbox_active_pressed);
		break;
	}
	case CHECKBOXSTATE::CHECKBOXUNACTIVE:
	{
		if (sprite_checkbox_unactive == nullptr) {
			LOG(LogType::L_WARNING, "The sprite 'U' is nullptr");
			return;
		}

		C_Image2D* img = static_cast<C_Image2D*>(gameObject->GetComponent(TYPE::IMAGE_2D));

		if (img != nullptr)
			img->SetTexture(sprite_checkbox_unactive);
		
		break;
	}
	case CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED:
	{
		if (sprite_checkbox_unactive_hovered == nullptr) {
			LOG(LogType::L_WARNING, "The sprite 'UH' is nullptr");
			return;
		}

		C_Image2D* img = static_cast<C_Image2D*>(gameObject->GetComponent(TYPE::IMAGE_2D));

		if (img != nullptr)
			img->SetTexture(sprite_checkbox_unactive_hovered);

		break;
	}
	case CHECKBOXSTATE::CHECKBOXUNACTIVEPRESSED:
	{
		if (sprite_checkbox_unactive_pressed == nullptr) {
			LOG(LogType::L_WARNING, "The sprite 'UP' is nullptr");
			return;
		}

		C_Image2D* img = static_cast<C_Image2D*>(gameObject->GetComponent(TYPE::IMAGE_2D));

		if (img != nullptr)
			img->SetTexture(sprite_checkbox_unactive_pressed);
		break;
	}
	}
}

#ifndef STANDALONE
void C_Checkbox::ChangeSprite(CHECKBOXSTATE num_sprite, ResourceTexture* sprite)
{
	switch (num_sprite)
	{
	case CHECKBOXSTATE::CHECKBOXACTIVE:
		if (sprite_checkbox_active != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite_checkbox_active->GetUID());
		}
		sprite_checkbox_active = sprite;
		break;
	case CHECKBOXSTATE::CHECKBOXACTIVEHOVERED:
		if (sprite_checkbox_active_hovered != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite_checkbox_active_hovered->GetUID());
		}
		sprite_checkbox_active_hovered = sprite;
		break;
	case CHECKBOXSTATE::CHECKBOXACTIVEPRESSED:
		if (sprite_checkbox_active_pressed != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite_checkbox_active_pressed->GetUID());
		}
		sprite_checkbox_active_pressed = sprite;
		break;
	case CHECKBOXSTATE::CHECKBOXUNACTIVE:
		if (sprite_checkbox_unactive != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite_checkbox_unactive->GetUID());
		}
		sprite_checkbox_unactive = sprite;
		break;
	case CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED:
		if (sprite_checkbox_unactive_hovered != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite_checkbox_unactive_hovered->GetUID());
		}
		sprite_checkbox_unactive_hovered = sprite;
		break;
	case CHECKBOXSTATE::CHECKBOXUNACTIVEPRESSED:
		if (sprite_checkbox_unactive_pressed != nullptr) {
			EngineExternal->moduleResources->UnloadResource(sprite_checkbox_unactive_pressed->GetUID());
		}
		sprite_checkbox_unactive_pressed = sprite;
		break;
	}
}

void C_Checkbox::ChangeScript(C_Script* script)
{
	if (script != nullptr);

}

bool C_Checkbox::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();
		if (sprite_checkbox_active != nullptr) {
			ImGui::Text("%s", sprite_checkbox_active->GetAssetPath());
		}
		ImGui::Columns(2);
		ImGui::Text("Drop here to change sprite 'A'");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				std::string assetsPath = *(std::string*)payload->Data;
				std::string str_name = "";
				FileSystem::SplitFilePath(assetsPath.c_str(), &assetsPath, &str_name);
				assetsPath += str_name;

				ChangeSprite(CHECKBOXSTATE::CHECKBOXACTIVE, dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath.c_str())));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();
		if (ImGui::Button("Edit Sprite 'A'")) {
			if (sprite_checkbox_active == nullptr) {
				LOG(LogType::L_WARNING, "The sprite 'A' is nullptr");
			}
			else {
				ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVE);
			}
		}
		ImGui::Columns(1);
		ImGui::Separator();

		if (sprite_checkbox_active_hovered != nullptr) {
			ImGui::Text("%s", sprite_checkbox_active_hovered->GetAssetPath());
		}

		ImGui::Columns(2);

		ImGui::Text("Drop here to change sprite 'AH'");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				std::string assetsPath = *(std::string*)payload->Data;
				std::string str_name = "";
				FileSystem::SplitFilePath(assetsPath.c_str(), &assetsPath, &str_name);
				assetsPath += str_name;

				ChangeSprite(CHECKBOXSTATE::CHECKBOXACTIVEHOVERED, dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath.c_str())));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		if (ImGui::Button("Edit Sprite 'AH'")) {
			if (sprite_checkbox_active_hovered == nullptr) {
				LOG(LogType::L_WARNING, "The sprite 'AH' is nullptr");
			}
			else {
				ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVEHOVERED);
			}
		}

		ImGui::Columns(1);
		ImGui::Separator();

		if (sprite_checkbox_active_pressed != nullptr) {
			ImGui::Text("%s", sprite_checkbox_active_pressed->GetAssetPath());
		}

		ImGui::Columns(2);

		ImGui::Text("Drop here to change sprite 'AP'");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				std::string assetsPath = *(std::string*)payload->Data;
				std::string str_name = "";
				FileSystem::SplitFilePath(assetsPath.c_str(), &assetsPath, &str_name);
				assetsPath += str_name;

				ChangeSprite(CHECKBOXSTATE::CHECKBOXACTIVEPRESSED, dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath.c_str())));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		if (ImGui::Button("Edit Sprite 'AP'")) {
			if (sprite_checkbox_active_pressed == nullptr) {
				LOG(LogType::L_WARNING, "The sprite 'AP' is nullptr");
			}
			else {
				ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVEPRESSED);
			}
		}

		ImGui::Columns(1);
		ImGui::Separator();

		if (sprite_checkbox_unactive != nullptr) {
			ImGui::Text("%s", sprite_checkbox_unactive->GetAssetPath());
		}

		ImGui::Columns(2);

		ImGui::Text("Drop here to change sprite 'U'");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				std::string assetsPath = *(std::string*)payload->Data;
				std::string str_name = "";
				FileSystem::SplitFilePath(assetsPath.c_str(), &assetsPath, &str_name);
				assetsPath += str_name;

				ChangeSprite(CHECKBOXSTATE::CHECKBOXUNACTIVE, dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath.c_str())));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		if (ImGui::Button("Edit Sprite 'U'")) {
			if (sprite_checkbox_unactive == nullptr) {
				LOG(LogType::L_WARNING, "The sprite 'U' is nullptr");
			}
			else {
				ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVE);
			}
		}

		ImGui::Columns(1);
		ImGui::Separator();

		if (sprite_checkbox_unactive_hovered != nullptr) {
			ImGui::Text("%s", sprite_checkbox_unactive_hovered->GetAssetPath());
		}

		ImGui::Columns(2);

		ImGui::Text("Drop here to change sprite 'UH'");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				std::string assetsPath = *(std::string*)payload->Data;
				std::string str_name = "";
				FileSystem::SplitFilePath(assetsPath.c_str(), &assetsPath, &str_name);
				assetsPath += str_name;

				ChangeSprite(CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED, dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath.c_str())));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		if (ImGui::Button("Edit Sprite 'UH'")) {
			if (sprite_checkbox_unactive_hovered == nullptr) {
				LOG(LogType::L_WARNING, "The sprite 'UH' is nullptr");
			}
			else {
				ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED);
			}
		}

		ImGui::Columns(1);
		ImGui::Separator();

		if (sprite_checkbox_unactive_pressed != nullptr) {
			ImGui::Text("%s", sprite_checkbox_unactive_pressed->GetAssetPath());
		}

		ImGui::Columns(2);

		ImGui::Text("Drop here to change sprite 'UP'");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
			{
				std::string assetsPath = *(std::string*)payload->Data;
				std::string str_name = "";
				FileSystem::SplitFilePath(assetsPath.c_str(), &assetsPath, &str_name);
				assetsPath += str_name;

				ChangeSprite(CHECKBOXSTATE::CHECKBOXUNACTIVEPRESSED, dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath.c_str())));
			}
			ImGui::EndDragDropTarget();
		}
		ImGui::NextColumn();

		if (ImGui::Button("Edit Sprite 'UP'")) {
			if (sprite_checkbox_unactive_pressed == nullptr) {
				LOG(LogType::L_WARNING, "The sprite 'UP' is nullptr");
			}
			else {
				ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVEPRESSED);
			}
		}

		ImGui::Columns(1);
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

#endif // !STANDALONE