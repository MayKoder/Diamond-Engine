#ifndef STANDALONE

#include "WI_Inspector.h"
#include"WI_Assets.h"

#include "MMGui.h"
#include "GameObject.h"
#include "Application.h"

#include "MO_Scene.h"
#include"MO_ResourceManager.h"
#include"MO_MonoManager.h"

#include"CO_Script.h"
#include"RE_Material.h"

W_Inspector::W_Inspector() : Window(), selectedGO(nullptr), editingRes(nullptr)
{
	name = "Inspector";
}

W_Inspector::~W_Inspector()
{
	if (editingRes)
		EngineExternal->moduleResources->UnloadResource(editingRes->GetUID());

}

void W_Inspector::Draw()
{

	//ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(1000, 1000));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*/)) 
	{
		if (editingRes != nullptr && editingRes->GetType() == Resource::Type::MATERIAL)
		{
			dynamic_cast<ResourceMaterial*>(editingRes)->DrawEditor();
		}
		else
		{
			if (selectedGO != nullptr && !selectedGO->IsRoot()) 
		{
			if (ImGui::Checkbox("##Active", &selectedGO->active)) 
			{
				//The bool has changed on the checkbox call at this point
				(selectedGO->active == true) ? selectedGO->Enable() : selectedGO->Disable();
			}

			ImGui::SameLine();
			
			strcpy(inputName, selectedGO->name.c_str());
			if (ImGui::InputText("##Name", &inputName[0], sizeof(inputName)) )
			{
				if(inputName[0] != '\0')
					selectedGO->name = inputName;
			}
			ImGui::SameLine();


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
				selectedGO->Destroy();
			}

			ImGui::GreySeparator();

			for (size_t i = 0; i < selectedGO->components.size(); i++)
			{
				selectedGO->components[i]->OnEditor();
			}

			ImGui::Separator();

			ImGui::CenterNextElement(ImGui::GetWindowSize(), 0.5f);		
			if (ImGui::BeginCombo("##addComponent", "Add Component"))
			{
				if (ImGui::Selectable("Mesh Renderer"))
				{
					if (selectedGO->GetComponent(Component::Type::MeshRenderer) == nullptr)
						selectedGO->AddComponent(Component::Type::MeshRenderer);
				}
				if (ImGui::Selectable("Material"))
				{
					if (selectedGO->GetComponent(Component::Type::Material) == nullptr)
						selectedGO->AddComponent(Component::Type::Material);
				}		
				if (ImGui::Selectable("Camera")) 
				{
					if(selectedGO->GetComponent(Component::Type::Camera) == nullptr)
						selectedGO->AddComponent(Component::Type::Camera);
				}
				if (ImGui::Selectable("Directional Light"))
				{
					if (selectedGO->GetComponent(Component::Type::DIRECTIONAL_LIGHT) == nullptr)
						selectedGO->AddComponent(Component::Type::DIRECTIONAL_LIGHT);
				}

				for (int i = 0; i < EngineExternal->moduleMono->userScripts.size(); i++)
				{
					if (ImGui::Selectable(mono_class_get_name(EngineExternal->moduleMono->userScripts[i]))) 
					{
						const char* name = mono_class_get_name(EngineExternal->moduleMono->userScripts[i]);
						C_Script* cs = dynamic_cast<C_Script*>(selectedGO->AddComponent(Component::Type::Script, name));
					}
				}


				ImGui::EndCombo();
			}

		}
		}

	}


	ImGui::End();
}

void W_Inspector::SetEditingResource(Resource* res)
{
	if (editingRes != nullptr)
		EngineExternal->moduleResources->UnloadResource(editingRes->GetUID());

	editingRes = res;

}

#endif // !STANDALONE