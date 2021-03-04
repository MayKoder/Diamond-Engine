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


		if (ImGui::Checkbox("Is this UI component selected?", &is_selected)) {
			if (is_selected)
				Select();
			else
				EngineExternal->moduleGui->uid_gameobject_of_ui_selected = 0;
		}
		ImGui::Columns(3);


		ImGui::Text("Button A:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button B:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button X:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Y:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Back:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Home:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Start:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Left Trigger:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Right Trigger:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Left Shoulder:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Button Right Shoulder:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("DPad Up:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("DPad Down:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("DPad Left:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("DPad Right:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Right Joystick Up:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Right Joystick Down:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Right Joystick Left:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Right Joystick Right:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Left Joystick Up:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Left Joystick Down:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Left Joystick Left:");
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::NextColumn();
		ImGui::Text("Left Joystick Right:");
		ImGui::Columns(1);



		
		//if (ImGui::BeginDragDropTarget())
		//{
		//	if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
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

	}

	return true;
}

#endif // !STANDALONE

