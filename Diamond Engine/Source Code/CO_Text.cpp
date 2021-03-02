#include "CO_Text.h"

#include "Application.h"

#include "MO_FileSystem.h"

#include "IM_FontImporter.h"
#include <map>

#include "ImGui/imgui.h"

C_Text::C_Text(GameObject* gameObject):Component(gameObject),font_path(""),text_to_print("")
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
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_FONT"))
			{
				const char* possible_new_font_path = ((std::string*)payload->Data)->c_str();
				if (!EngineExternal->moduleFileSystem->free_type_library->CheckIfFontExists(possible_new_font_path)) {
					LOG(LogType::L_WARNING, "This file is not a font file");
				}
				else {
					font_path = possible_new_font_path;
				}

			}
			ImGui::EndDragDropTarget();
		}
		ImGui::InputText("Text to print", &text_to_print[0], sizeof(text_to_print));

	}
	return true;
}
#endif // !STANDALONE


