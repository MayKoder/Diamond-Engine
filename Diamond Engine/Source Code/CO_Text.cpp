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

#ifndef STANDALONE
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
				const char* possible_new_font_path = ((std::string*)payload->Data)->c_str();
				int possible_new_font_id = EngineExternal->moduleFileSystem->free_type_library->GetFont(possible_new_font_path);
				if (possible_new_font_id < 0) {
					LOG(LogType::L_WARNING, "This file is not a font file");
				}
				else {
					font_id = possible_new_font_id;
					font_path = possible_new_font_path;
				}

			}
			ImGui::EndDragDropTarget();
		}


	}
	return true;
}
#endif // !STANDALONE


