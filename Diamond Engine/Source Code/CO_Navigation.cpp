#include "CO_Navigation.h"

#include "Application.h"
#include "MO_GUI.h"
#include "MO_Scene.h"

#include "GameObject.h"

#include "ImGui/imgui.h"

C_Navigation::C_Navigation(GameObject* gameObject) :Component(gameObject), is_selected(false)
{
	map_of_buttons_and_joysticks.clear();
	name = "Navigation";
}

C_Navigation::~C_Navigation()
{
	map_of_buttons_and_joysticks.clear();
}

void C_Navigation::Update()
{
	if (!is_selected)
		return;
	if (gameObject->GetComponent(Component::TYPE::CHECKBOX) != nullptr) {
		LOG(LogType::L_NORMAL, "HEY");
	}
}

void C_Navigation::Select()
{
	if (EngineExternal->moduleGui->uid_gameobject_of_ui_selected != 0) {
		GameObject* previous_selected = EngineExternal->moduleScene->GetGOFromUID(EngineExternal->moduleScene->root, EngineExternal->moduleGui->uid_gameobject_of_ui_selected);
		static_cast<C_Navigation*>(previous_selected->GetComponent(Component::TYPE::NAVIGATION))->is_selected = false;
	}
	EngineExternal->moduleGui->uid_gameobject_of_ui_selected = gameObject->UID;
	is_selected = true;
}

#ifndef STANDALONE
bool C_Navigation::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Separator();

		
		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TEXTURE"))
		//	{
		//		//Drop asset from Asset window to scene window
		//		std::string* metaFileDrop = (std::string*)payload->Data;

		//		if (texture != nullptr)
		//			EngineExternal->moduleResources->UnloadResource(texture->GetUID());

		//		std::string libraryName = EngineExternal->moduleResources->LibraryFromMeta(metaFileDrop->c_str());

		//		texture = dynamic_cast<ResourceTexture*>(EngineExternal->moduleResources->RequestResource(EngineExternal->moduleResources->GetMetaUID(metaFileDrop->c_str()), libraryName.c_str()));
		//		LOG(LogType::L_WARNING, "File %s loaded to scene", (*metaFileDrop).c_str());
		//	}
		//	ImGui::EndDragDropTarget();
		//}

		if (ImGui::Checkbox("Is this UI component selected?", &is_selected)) {
			if (is_selected)
				Select();
			else
				EngineExternal->moduleGui->uid_gameobject_of_ui_selected = 0;
		}
	}

	return true;
}

#endif // !STANDALONE

