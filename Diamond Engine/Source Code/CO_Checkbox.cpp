#include "CO_Checkbox.h"
#include "CO_Script.h"
#include "CO_Image2D.h"

#include "RE_Texture.h"

#include "GameObject.h"

#include "Application.h"
#include "MO_ResourceManager.h"

#include "IM_FileSystem.h"

#include "ImGui/imgui.h"


C_Checkbox::C_Checkbox(GameObject* gameObject): Component(gameObject), sprite_checkbox_active(nullptr), sprite_checkbox_active_hovered(nullptr), sprite_checkbox_active_pressed(nullptr),
sprite_checkbox_unactive(nullptr), sprite_checkbox_unactive_hovered(nullptr), sprite_checkbox_unactive_pressed(nullptr), script_name(""), num_sprite_used(CHECKBOXSTATE::CHECKBOXUNACTIVE),
checkbox_active(false), is_selected(false)
{
	name = "Checkbox";
#ifndef STANDALONE
	sprites_freezed = false;
#endif // !STANDALONE
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
	if (gameObject->GetComponent(Component::TYPE::SCRIPT, script_name.c_str()) == nullptr)
		script_name = "";
	if (sprites_freezed)
		return;
#endif // !STANDALONE


	switch (num_sprite_used)
	{
	case CHECKBOXSTATE::CHECKBOXACTIVE:
		if (is_selected)
			ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVEHOVERED);
		break;
	case CHECKBOXSTATE::CHECKBOXACTIVEHOVERED:
		if (!is_selected)
			ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVE);
		break;
	case CHECKBOXSTATE::CHECKBOXUNACTIVE:
		if (is_selected)
			ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED);
		break;
	case CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED:
		if (!is_selected)
			ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVE);
		break;
	}

}

void C_Checkbox::PressCheckbox()
{
	checkbox_active = !checkbox_active;
	if (checkbox_active) {
		ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVEPRESSED);
		C_Script* script = static_cast<C_Script*>(gameObject->GetComponent(Component::TYPE::SCRIPT, script_name.c_str()));
		if (script != nullptr)
			script->ExecuteCheckbox(checkbox_active);
	}
	else {
		ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVEPRESSED);
		C_Script* script = static_cast<C_Script*>(gameObject->GetComponent(Component::TYPE::SCRIPT, script_name.c_str()));
		if (script != nullptr)
			script->ExecuteCheckbox(checkbox_active);
	}
}

void C_Checkbox::UnpressCheckbox()
{
	if (checkbox_active) {
		if (is_selected)
			ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVEHOVERED);
		else
			ChangeTexture(CHECKBOXSTATE::CHECKBOXACTIVE);
	}
	else {
		if (is_selected)
			ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVEHOVERED);
		else
			ChangeTexture(CHECKBOXSTATE::CHECKBOXUNACTIVE);
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
			return;
		}

		C_Image2D* img = static_cast<C_Image2D*>(gameObject->GetComponent(TYPE::IMAGE_2D));

		if (img != nullptr)
			img->SetTexture(sprite_checkbox_unactive_pressed);
		break;
	}
	}
}

void C_Checkbox::SaveData(JSON_Object* nObj)
{
	Component::SaveData(nObj);

	if (sprite_checkbox_active_pressed != nullptr)
	{
		DEJson::WriteString(nObj, "Active_Pressed_AssetsPath", sprite_checkbox_active_pressed->GetAssetPath());
		DEJson::WriteString(nObj, "Active_Pressed_LibraryPath", sprite_checkbox_active_pressed->GetLibraryPath());
		DEJson::WriteInt(nObj, "Active_Pressed_UID", sprite_checkbox_active_pressed->GetUID());
	}
	if (sprite_checkbox_active_hovered != nullptr)
	{
		DEJson::WriteString(nObj, "Active_Hovered_AssetsPath", sprite_checkbox_active_hovered->GetAssetPath());
		DEJson::WriteString(nObj, "Active_Hovered_LibraryPath", sprite_checkbox_active_hovered->GetLibraryPath());
		DEJson::WriteInt(nObj, "Active_Hovered_UID", sprite_checkbox_active_hovered->GetUID());
	}
	if (sprite_checkbox_active != nullptr)
	{
		DEJson::WriteString(nObj, "Active_AssetsPath", sprite_checkbox_active->GetAssetPath());
		DEJson::WriteString(nObj, "Active_LibraryPath", sprite_checkbox_active->GetLibraryPath());
		DEJson::WriteInt(nObj, "Active_UID", sprite_checkbox_active->GetUID());
	}

	if (sprite_checkbox_unactive_pressed != nullptr)
	{
		DEJson::WriteString(nObj, "Unactive_Pressed_AssetsPath", sprite_checkbox_unactive_pressed->GetAssetPath());
		DEJson::WriteString(nObj, "Unactive_Pressed_LibraryPath", sprite_checkbox_unactive_pressed->GetLibraryPath());
		DEJson::WriteInt(nObj, "Unactive_Pressed_UID", sprite_checkbox_unactive_pressed->GetUID());
	}
	if (sprite_checkbox_unactive_hovered != nullptr)
	{
		DEJson::WriteString(nObj, "Unactive_Hovered_AssetsPath", sprite_checkbox_unactive_hovered->GetAssetPath());
		DEJson::WriteString(nObj, "Unactive_Hovered_LibraryPath", sprite_checkbox_unactive_hovered->GetLibraryPath());
		DEJson::WriteInt(nObj, "Unactive_Hovered_UID", sprite_checkbox_unactive_hovered->GetUID());
	}
	if (sprite_checkbox_unactive != nullptr)
	{
		DEJson::WriteString(nObj, "Unactive_AssetsPath", sprite_checkbox_unactive->GetAssetPath());
		DEJson::WriteString(nObj, "Unactive_LibraryPath", sprite_checkbox_unactive->GetLibraryPath());
		DEJson::WriteInt(nObj, "Unactive_UID", sprite_checkbox_unactive->GetUID());
	}

	if (!script_name.empty())
	{
		DEJson::WriteString(nObj, "Script_Name", script_name.c_str());
	}
	DEJson::WriteInt(nObj, "ButtonState", static_cast<int>(num_sprite_used));
	DEJson::WriteBool(nObj, "Checkbox Active", checkbox_active);
	DEJson::WriteBool(nObj, "Is Selected", is_selected);
}

void C_Checkbox::LoadData(DEConfig& nObj)
{
	Component::LoadData(nObj);

	std::string texName = nObj.ReadString("Active_Pressed_LibraryPath");
	std::string assetsName = nObj.ReadString("Active_Pressed_AssetsPath");

	if (texName != "") {
		sprite_checkbox_active_pressed = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("Active_Pressed_UID"), texName.c_str()));
		sprite_checkbox_active_pressed->SetAssetsPath(assetsName.c_str());
	}

	texName = nObj.ReadString("Active_Hovered_LibraryPath");
	assetsName = nObj.ReadString("Active_Hovered_AssetsPath");

	if (texName != "") {
		sprite_checkbox_active_hovered = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("Active_Hovered_UID"), texName.c_str()));
		sprite_checkbox_active_hovered->SetAssetsPath(assetsName.c_str());
	}


	texName = nObj.ReadString("Active_LibraryPath");
	assetsName = nObj.ReadString("Active_AssetsPath");

	if (texName != "") {
		sprite_checkbox_active = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("Active_UID"), texName.c_str()));
		sprite_checkbox_active->SetAssetsPath(assetsName.c_str());
	}

	texName = nObj.ReadString("Unactive_Pressed_LibraryPath");
	assetsName = nObj.ReadString("Unactive_Pressed_AssetsPath");

	if (texName != "") {
		sprite_checkbox_unactive_pressed = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("Unactive_Pressed_UID"), texName.c_str()));
		sprite_checkbox_unactive_pressed->SetAssetsPath(assetsName.c_str());
	}

	texName = nObj.ReadString("Unactive_Hovered_LibraryPath");
	assetsName = nObj.ReadString("Unactive_Hovered_AssetsPath");

	if (texName != "") {
		sprite_checkbox_unactive_hovered = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("Unactive_Hovered_UID"), texName.c_str()));
		sprite_checkbox_unactive_hovered->SetAssetsPath(assetsName.c_str());
	}


	texName = nObj.ReadString("Unactive_LibraryPath");
	assetsName = nObj.ReadString("Unactive_AssetsPath");

	if (texName != "") {
		sprite_checkbox_unactive = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(nObj.ReadInt("Unactive_UID"), texName.c_str()));
		sprite_checkbox_unactive->SetAssetsPath(assetsName.c_str());
	}

	texName = nObj.ReadString("Script_Name");

	if (texName != "")
		script_name = nObj.ReadString("Script_Name");

	num_sprite_used = static_cast<CHECKBOXSTATE>(nObj.ReadInt("ButtonState"));
	checkbox_active = nObj.ReadBool("Checkbox Active");
	is_selected = nObj.ReadBool("Is Selected");
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

void C_Checkbox::ChangeScript(const char* new_script_name)
{
	if (!script_name.empty()) {
		Component* component = gameObject->GetComponent(Component::TYPE::SCRIPT, script_name.c_str());
		if (gameObject != nullptr)
			gameObject->RemoveComponent(component);
	}
	dynamic_cast<C_Script*>(gameObject->AddComponent(TYPE::SCRIPT, new_script_name));

	script_name = new_script_name;

}

bool C_Checkbox::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();
		ImGui::Checkbox("Freeze sprites", &sprites_freezed);

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

		ImGui::Text(script_name.c_str());

		ImGui::Text("Drop here to change the script");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_SCRIPT"))
			{
				std::string* assetsPath = (std::string*)payload->Data;
				std::string file_name;
				FileSystem::GetFileName(assetsPath->c_str(), file_name, false);

				ChangeScript(file_name.c_str());
			}
			ImGui::EndDragDropTarget();
		}


	}
	return true;
}

#endif // !STANDALONE