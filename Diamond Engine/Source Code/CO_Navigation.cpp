#include "CO_Navigation.h"

#include "Application.h"
#include "MO_GUI.h"
#include "MO_Scene.h"

#include "GameObject.h"

#include "ImGui/imgui.h"

#include <assert.h>

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

		WriteButtonOrJoystickOnEditor("Button A:", BUTTONSANDJOYSTICKS::BUTTON_A);
		
		WriteButtonOrJoystickOnEditor("Button B:", BUTTONSANDJOYSTICKS::BUTTON_B);

		WriteButtonOrJoystickOnEditor("Button X:", BUTTONSANDJOYSTICKS::BUTTON_X);

		WriteButtonOrJoystickOnEditor("Button Y:", BUTTONSANDJOYSTICKS::BUTTON_Y);
		
		WriteButtonOrJoystickOnEditor("Button Back:", BUTTONSANDJOYSTICKS::BUTTON_BACK);

		WriteButtonOrJoystickOnEditor("Button Home:", BUTTONSANDJOYSTICKS::BUTTON_GUIDE);

		WriteButtonOrJoystickOnEditor("Button Start:", BUTTONSANDJOYSTICKS::BUTTON_START);
		
		WriteButtonOrJoystickOnEditor("Button Left Trigger:", BUTTONSANDJOYSTICKS::BUTTON_LEFTTTRIGGER);

		WriteButtonOrJoystickOnEditor("Button Right Trigger:", BUTTONSANDJOYSTICKS::BUTTON_RIGHTTRIGGER);

		WriteButtonOrJoystickOnEditor("Button Left Shoulder:", BUTTONSANDJOYSTICKS::BUTTON_LEFTSHOULDER);

		WriteButtonOrJoystickOnEditor("Button Right Shoulder:", BUTTONSANDJOYSTICKS::BUTTON_RIGHTSHOULDER);

		WriteButtonOrJoystickOnEditor("DPad Up:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_UP);

		WriteButtonOrJoystickOnEditor("DPad Down:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_DOWN);

		WriteButtonOrJoystickOnEditor("DPad Left:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_LEFT);

		WriteButtonOrJoystickOnEditor("DPad Right:", BUTTONSANDJOYSTICKS::BUTTON_DPAD_RIGHT);

		WriteButtonOrJoystickOnEditor("Right Joystick Up:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_UP);

		WriteButtonOrJoystickOnEditor("Right Joystick Down:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_DOWN);

		WriteButtonOrJoystickOnEditor("Right Joystick Left:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_LEFT);

		WriteButtonOrJoystickOnEditor("Right Joystick Right:", BUTTONSANDJOYSTICKS::RIGHT_JOYSTICK_RIGHT);

		WriteButtonOrJoystickOnEditor("Left Joystick Up:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_UP);

		WriteButtonOrJoystickOnEditor("Left Joystick Down:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_DOWN);

		WriteButtonOrJoystickOnEditor("Left Joystick Left:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_LEFT);
		
		WriteButtonOrJoystickOnEditor("Left Joystick Right:", BUTTONSANDJOYSTICKS::LEFT_JOYSTICK_RIGHT);

		ImGui::Columns(1);



		
		

	}

	return true;
}

void C_Navigation::WriteButtonOrJoystickOnEditor(const char* text, BUTTONSANDJOYSTICKS button_or_joystick)
{
	ImGui::Text(text);
	ImGui::NextColumn();
	std::string text_string = "##";
	text_string += static_cast<int>(button_or_joystick);
	assert(static_cast<int>(ACTIONSNAVIGATION::TOTAL_ACTIONS_WITH_NONE) == 3, "Update the references of ACTIONSNAVIGATION");
	const char* items[] = { "None", "Move", "Execute" };
	const char* current_item = items[0];
	if (map_of_buttons_and_joysticks.count(button_or_joystick) != 0) {
		current_item = items[static_cast<int>(map_of_buttons_and_joysticks[button_or_joystick].action)];
	}
	if (ImGui::BeginCombo(text_string.c_str(), current_item))
	{
		for (int n = 0; n < IM_ARRAYSIZE(items); n++)
		{
			bool is_selected = (current_item == items[n]);
			if (ImGui::Selectable(items[n], is_selected)) {
				current_item = items[n];
				if (map_of_buttons_and_joysticks.count(button_or_joystick) != 0) {
					if (n == 0) {
						map_of_buttons_and_joysticks.erase(button_or_joystick);
					}
					else
						map_of_buttons_and_joysticks[button_or_joystick].action = static_cast<ACTIONSNAVIGATION>(n);
				}
				else {
					ActionToRealize action;
					action.action = static_cast<ACTIONSNAVIGATION>(n);
					action.uid_gameobject = 0;
					map_of_buttons_and_joysticks.emplace(button_or_joystick, action);
				}
			}
			if (is_selected)
				ImGui::SetItemDefaultFocus();
		}
		ImGui::EndCombo();
	}
	ImGui::NextColumn();
	if (current_item != items[0]) {
		ImGui::Text("UID:"); 
		ImGui::SameLine();
		ImGui::Text(std::to_string(map_of_buttons_and_joysticks[button_or_joystick].uid_gameobject).c_str());
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_GAMEOBJECT"))
			{
				map_of_buttons_and_joysticks[button_or_joystick].uid_gameobject = *(int*)payload->Data;
			}
			ImGui::EndDragDropTarget();
		}
	}
	ImGui::NextColumn();
}


#endif // !STANDALONE

