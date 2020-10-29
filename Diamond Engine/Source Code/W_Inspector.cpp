#include "W_Inspector.h"
#include "MMGui.h"
#include "GameObject.h"
#include "Application.h"
#include "M_Scene.h"

W_Inspector::W_Inspector() : Window(), selectedGO(nullptr)
{
	name = "Inspector";
}

W_Inspector::~W_Inspector()
{
}

void W_Inspector::Draw()
{
	char inputName[12] = "Object Name";

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(1000, 1000));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*/)) 
	{
		if (selectedGO != nullptr && selectedGO != EngineExternal->moduleScene->root) 
		{
			if (ImGui::Checkbox("##Active", &selectedGO->active)) 
			{
				//The bool has changed on the checkbox call at this point
				(selectedGO->active == true) ? selectedGO->Enable() : selectedGO->Disable();
			}

			ImGui::SameLine();
			ImGui::InputText("##Name", inputName, sizeof(inputName) / sizeof(char)); ImGui::SameLine();
			ImGui::Checkbox("Static", &selectedGO->isStatic);

			ImGui::Text("Tag"); ImGui::SameLine();
			const char* items[] = { "AAAA", "BBBB", "CCCC", "DDDD", "EEEE", "FFFF", "GGGG", "HHHH", "IIII", "JJJJ", "KKKK", "LLLLLLL", "MMMM", "OOOOOOO", "PPPP", "QQQQQQQQQQ", "RRR", "SSSS" };
			static const char* current_item = NULL;

			ImGuiStyle& style = ImGui::GetStyle();
			float w = ImGui::CalcItemWidth();
			float spacing = style.ItemInnerSpacing.x;
			float button_sz = ImGui::GetFrameHeight();
			ImGui::PushItemWidth((w - spacing * 2.0f - button_sz * 2.0f) * 0.5f);
			if (ImGui::BeginCombo("##custom combo", current_item))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (current_item == items[n]);
					if (ImGui::Selectable(items[n], is_selected))
						current_item = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			ImGui::SameLine();

			ImGui::Text("Layer"); ImGui::SameLine();
			 style = ImGui::GetStyle();
			 w = ImGui::CalcItemWidth();
			 spacing = style.ItemInnerSpacing.x;
			 button_sz = ImGui::GetFrameHeight();
			ImGui::PushItemWidth((w - spacing * 2.0f - button_sz * 2.0f) * 0.5f);
			if (ImGui::BeginCombo("##custom combo 2", current_item))
			{
				for (int n = 0; n < IM_ARRAYSIZE(items); n++)
				{
					bool is_selected = (current_item == items[n]);
					if (ImGui::Selectable(items[n], is_selected))
						current_item = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			ImGui::PopItemWidth();

			if (ImGui::Button("Delete")) {
				EngineExternal->moduleScene->destroyList.push_back(selectedGO);
			}

			ImGui::GreySeparator();

			for (size_t i = 0; i < selectedGO->components.size(); i++)
			{
				selectedGO->components[i]->OnEditor();
			}

		}
	}


	ImGui::End();

	//ImGui::PopStyleVar();

}
