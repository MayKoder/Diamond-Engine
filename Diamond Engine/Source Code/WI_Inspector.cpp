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

//todelete
#include "Application.h"
#include "MO_Physics.h"
#include "IM_PrefabImporter.h"

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
				selectedGO->Destroy();}

			ImGui::SameLine();
			ImGui::Text("UID: %d", selectedGO->UID);

			if (selectedGO->prefabID != 0u) 
			{
				ImGui::Text("Prefab ID: %d", selectedGO->prefabID);
				if (ImGui::Button("Override Prefab")){
					PrefabImporter::OverridePrefabGameObjects(selectedGO->prefabID, selectedGO);
				}

				ImGui::SameLine();
				if (ImGui::Button("Revert Changes")) {
					PrefabImporter::OverrideGameObject(selectedGO->prefabID, selectedGO);
				}
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
					if (selectedGO->GetComponent(Component::TYPE::MESH_RENDERER) == nullptr)
						selectedGO->AddComponent(Component::TYPE::MESH_RENDERER);
				}
				if (ImGui::Selectable("Material"))
				{
					if (selectedGO->GetComponent(Component::TYPE::MATERIAL) == nullptr)
						selectedGO->AddComponent(Component::TYPE::MATERIAL);
				}		
				if (ImGui::Selectable("Camera")) 
				{
					if(selectedGO->GetComponent(Component::TYPE::CAMERA) == nullptr)
						selectedGO->AddComponent(Component::TYPE::CAMERA);
				}
				if (ImGui::Selectable("RigidBody3D"))
				{
					if (selectedGO->GetComponent(Component::TYPE::RIGIDBODY) == nullptr)
						selectedGO->AddComponent(Component::TYPE::RIGIDBODY);
				}
				if (ImGui::Selectable("Box Collider"))
				{
					if (selectedGO->GetComponent(Component::TYPE::BOXCOLLIDER) == nullptr)
						selectedGO->AddComponent(Component::TYPE::BOXCOLLIDER);
				}
				if (ImGui::Selectable("Mesh Collider"))
				{
					if (selectedGO->GetComponent(Component::TYPE::MESHCOLLIDER) == nullptr)
						selectedGO->AddComponent(Component::TYPE::MESHCOLLIDER);

				}
				if (ImGui::Selectable("AudioListener"))
				{
					if (selectedGO->GetComponent(Component::TYPE::AUDIO_LISTENER) == nullptr)
						selectedGO->AddComponent(Component::TYPE::AUDIO_LISTENER);
				}
				if (ImGui::Selectable("AudioSource"))
				{
					if (selectedGO->GetComponent(Component::TYPE::AUDIO_SOURCE) == nullptr)
						selectedGO->AddComponent(Component::TYPE::AUDIO_SOURCE);
				}
				if (ImGui::Selectable("Animator"))
				{
					if (selectedGO->GetComponent(Component::TYPE::ANIMATOR) == nullptr)
						selectedGO->AddComponent(Component::TYPE::ANIMATOR);
				}
				
				for (int i = 0; i < EngineExternal->moduleMono->userScripts.size(); i++)
				{
					if (ImGui::Selectable(mono_class_get_name(EngineExternal->moduleMono->userScripts[i]))) 
					{
						const char* name = mono_class_get_name(EngineExternal->moduleMono->userScripts[i]);
						C_Script* cs = dynamic_cast<C_Script*>(selectedGO->AddComponent(Component::TYPE::SCRIPT, name));
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