#include "WI_TextEditor.h"
#include"IM_FileSystem.h"
#include"MO_MonoManager.h"

#include"PugiXML/pugixml.hpp"

W_TextEditor::W_TextEditor() : Window(), txtName(nullptr) /*: texColorBuffer(-1)*/
{
	name = "Text Editor"; //No lng definition for C# :(

	SetTextFromFile("Assets/Scripts/Core.cs");

	pugi::xml_document doc;
	pugi::xml_parse_result result = doc.load_file("Assembly-CSharp/Assembly-CSharp.csproj");
	//pugi::xml_parse_result result = doc.load_file("test.xml");

	if (result.status == pugi::xml_parse_status::status_file_not_found)
		assert(false, "XML File not loaded");

	pugi::xml_node whereToAdd = doc.child("Project");
	for (pugi::xml_node panel = whereToAdd.first_child(); panel != nullptr; panel = panel.next_sibling())
	{
		if (strcmp(panel.name(), "ItemGroup") == 0 && strcmp(panel.first_child().name(), "Compile") == 0) 
		{
			panel = panel.first_child();
			panel.append_child();
			panel.set_name("Compile");
			pugi::xml_attribute att = panel.append_attribute("Include");
			att.set_value("../Assets/Scripts/Hola.cs");

			panel = panel.append_child();
			panel.set_name("Link");
			panel.text().set("Hola.cs");

			break;
		}
	}

	doc.save_file("Assembly-CSharp/Assembly-CSharp.csproj");
}

W_TextEditor::~W_TextEditor()
{

}

void W_TextEditor::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*//*, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse*/))
	{
		if (ImGui::Button("Manualy recompile CS"))
			EngineExternal->moduleMono->ReCompileCS();

		txtEditor.Render(txtName);
	}
	ImGui::End();
	ImGui::PopStyleVar();

}

void W_TextEditor::SetTextFromFile(const char* path)
{
	txtName = path;

	std::string test = FileSystem::FileToText(path);
	txtEditor.SetText(test);
}
