#include "CO_Text.h"

#include "Application.h"

#include "MO_FileSystem.h"

#include "IM_FontImporter.h"

#include "ImGui/imgui.h"

C_Text::C_Text(GameObject* gameObject):Component(gameObject),font_id(-1),font_path("")
{
}

C_Text::~C_Text()
{
}

bool C_Text::OnEditor()
{
	if (Component::OnEditor() == true)
	{
		ImGui::Text("Path: %s", font_path);
		ImGui::Text("Drop here to change the path");
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_OTHER"))
			{
				font_path = ((std::string*)payload->Data)->c_str();
				font_id = EngineExternal->moduleFileSystem->free_type_library->GetFont(font_path);

				/*if (sprite3 != nullptr)
					EngineExternal->moduleResources->UnloadResource(sprite3->GetUID());

				sprite3 = dynamic_cast<ResourceMaterial*>(EngineExternal->moduleResources->RequestFromAssets(assetsPath->c_str()));*/
				
			}
			ImGui::EndDragDropTarget();
		}


	}
	return true;
}
