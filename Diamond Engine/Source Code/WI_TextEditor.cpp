#include "WI_TextEditor.h"
#include"IM_FileSystem.h"

W_TextEditor::W_TextEditor() : Window() /*: texColorBuffer(-1)*/
{
	name = "Text Editor"; //No lng definition for C# :(

	//txtEditor.SetText();
	std::string test = FileSystem::FileToText("CSSolution/Assembly-CSharp/Core.cs");
	txtEditor.SetText(test);
}

W_TextEditor::~W_TextEditor()
{

}

void W_TextEditor::Draw()
{
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));

	if (ImGui::Begin(name.c_str(), NULL /*| ImGuiWindowFlags_NoResize*//*, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse*/))
	{
		txtEditor.Render("Script");
	}
	ImGui::End();
	ImGui::PopStyleVar();

}